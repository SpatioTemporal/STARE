use master
GO

sp_addextendedproc  'xp_HTM_Lookup', 'SpatialIndex.dll'
GO

sp_addextendedproc  'xp_HTM_Cover', 'SpatialIndex.dll'
GO

----------------------------------------------------------
-- end of install of extended stored procedures in master db.
---------------------------------------------------------- 

USE gyuribase
GO
---------------------------------------
--
CREATE FUNCTION  fHTM_Lookup (@Coordinate VARCHAR(1000))
---------------------------------------------------------------------
--/H  Find the htmID of the specified point on the celestial sphere
---------------------------------------------------------------------
--/T  Coordinate syntax is: 
--/T  <li>J2000 depth ra dec <i>in spherical coordinates</i><br>
--/T  Example: <samp>J2000 20 240.0 38.0</samp>
--/T  <li>CARTESIAN depth x y z <i>where x, y, z are numbers giving the position of the point 
--/T  on the sphere, depth is an integer in the range 2..14 giving the mesh depth, 
--/T  ra, dec, x, y, z are floats. </i><br>
--/T  Example: <samp>CARTESIAN 9 1.0 1.0 1.0</samp>
--/T  <li>Example: 
--/T  <samp>select dbo.fHTM_Lookup('J2000 20 185 0')</samp>
--/T  <br>
----------------------------------------------------- 
RETURNS BIGINT
AS
BEGIN
	DECLARE @HTM_Lookup BIGINT	-- the answer
	DECLARE @HTM_KLUDGE BINARY(8) 	-- a workaround for the fact that 
				      	-- external stored procs do not take BIGINT params
	DECLARE @retcode    INT		--
	DECLARE @ErrorMsg   VARCHAR(1000) -- error messsage from lookup (if coordinate has syntax error)
	EXECUTE  @retcode = master.dbo.xp_HTM_Lookup @Coordinate, @HTM_KLUDGE OUTPUT, @ErrorMsg OUTPUT
	IF (@retcode = 0)   		-- got bytes, cast them as an INT64
		BEGIN
		SET @HTM_Lookup = CAST(substring(@HTM_KLUDGE, 1, 8) AS BIGINT)
		END
	ELSE
	   BEGIN SET @HTM_Lookup = -@retcode END
	RETURN(@HTM_Lookup) 
END					-- end of HTM_Lookup
GO	

--
CREATE FUNCTION   fHTM_Lookup_ErrorMessage (@Coordinate VARCHAR(1000))
----------------------------------------------------------------
--/H  Returns the error message that fHTM_Lookup generated
----------------------------------------------------------------
--/T  If there was no error, returns the string 'OK'.
--/T  <samp>fHTM_Lookup_ErrorMessage ('J2000  20 185 0')</samp>
--/T  <br> see fHTM_Lookup for definition of input param
--------------------------------------------------
RETURNS VARCHAR(1000)
AS
BEGIN
	DECLARE @HTM_KLUDGE BINARY(8) 	-- a workaround for the fact that 
				      	-- external stored procs do not take BIGINT params
	DECLARE @retcode    INT		--
	DECLARE @ErrorMsg   VARCHAR(1000) -- error messsage from lookup (if coordinate has syntax error)
	EXECUTE  @retcode = master.dbo.xp_HTM_Lookup @Coordinate, @HTM_KLUDGE OUTPUT, @ErrorMsg OUTPUT
	IF (@retcode != 0)   
		BEGIN
		SET @ErrorMsg = 'Call to master.dbo.xp_HTM_Lookup failed completely'
		END
	RETURN(@ErrorMsg) 
END
GO	 


IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_Cover')
	DROP FUNCTION fHTM_Cover 
GO
--

CREATE FUNCTION fHTM_Cover (@Area VARCHAR(8000))
RETURNS @Triangles TABLE (
		HTMIDstart BIGINT NOT NULL PRIMARY KEY,  
		HTMIDend   BIGINT NOT NULL)
AS
BEGIN
   DECLARE @Vector 	VARBINARY(8000)	-- the outpvector from xp_HTM 
   DECLARE @Elements 	INT		-- size of output vector (in bigints)
   DECLARE @Cell     	INT		-- index to output vector cell
   DECLARE @StartHTM 	BIGINT		-- value of first htm cell
   DECLARE @EndHTM 	BIGINT		-- value of second htm cell
   DECLARE @Retcode 	INT		-- retcode from xp_HTM.
   DECLARE @ErrorMsg	VARCHAR(1000)   -- error message from HTM code
   -- get a vector of up to 1,000 triangles covering the desired area (input params missing at present).

   EXECUTE  @retcode = master.dbo.xp_HTM_Cover @Area, @Vector OUTPUT, @ErrorMsg OUTPUT

   SET @Elements = DATALENGTH(@Vector)/8		-- each element is 8 bytes
   IF ((@Elements % 2) != 0) SET @Elements = 0  -- error if an odd number of elements
   SET @Cell = 0				--
   WHILE (@Cell < @Elements)			-- loop over array
	BEGIN					-- extracting pairs of cells
	SET @StartHTM = CAST (substring(@Vector, (8 * @Cell)+1, 8) AS BIGINT)
	SET @EndHTM = CAST (substring(@Vector, (8 * (@Cell+1))+1, 8) AS BIGINT)
   	INSERT @Triangles VALUES(@StartHTM, @EndHTM+1)	-- insert pair in answer table
 	SET @Cell = @Cell + 2			-- go to next pair
	END					-- end of loop to get 
   RETURN					--
END						-- end of fHTM
GO


--

CREATE FUNCTION fHTM_Cover_ErrorMessage (@Area VARCHAR(8000))
RETURNS VARCHAR(1000)
AS
BEGIN
   DECLARE @Vector 	VARBINARY(8000)	-- the outpvector from xp_HTM 
   DECLARE @retcode 	INT		-- retcode from xp_HTM.
   DECLARE @ErrorMesssage VARCHAR(1000)   -- error message from HTM code
   EXECUTE  @retcode = master.dbo.xp_HTM_Cover @Area, @Vector OUTPUT, @ErrorMesssage OUTPUT
	IF ((@retcode != 0) and (@retcode != 20001))   
	BEGIN
	SET @ErrorMesssage = 'Call to master.dbo.xp_HTM failed completely'
	END 
   RETURN(@ErrorMesssage) 
END						-- end of fHTM_ErrorMessage
GO
----

CREATE FUNCTION   fHTM_To_String (@HTM BIGINT)
RETURNS VARCHAR(1000)
AS
	BEGIN
	DECLARE @HTMtemp	BIGINT		-- eat away at @HTM as you parse it.
	DECLARE @Answer		VARCHAR(1000)	-- the answer string.
	DECLARE @Triangle 	INT		-- the triangle id (0..3)
	SET @Answer = ''			--
	SET @HTMtemp = @HTM			--
	------------------------------------------
	-- loop over the HTM pulling off a triangle till we have a faceid left (1...8)
	WHILE (@HTMtemp > 0)
		BEGIN
		IF (@HTMtemp <= 8) 		-- its a face
			BEGIN			-- add face to string.
			SET @Answer = CAST(@HTMtemp as VARCHAR(4)) + @Answer
			SET @HTMtemp  = 0
			END			-- end face case
		ELSE
			BEGIN			-- its a triangle
			SET @Triangle = @HTMtemp % 4	-- get the id into answer
			SET @Answer =  ',' +CAST(@Triangle as VARCHAR(4)) + @Answer
			SET @HTMTemp = @HTMtemp / 4 	-- move on to next triangle
			END			-- end triangle case
		END				-- end loop
	RETURN(@Answer) 			
END			
GO	

PRINT 'HTM functions created'
GO
