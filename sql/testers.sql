
use gyuribase
go

-- SELECT 'J2000     20 240.0 38.0 ' , dbo.fHTM_Lookup_ErrorMessage('J2000 20 240.0 38.0'     )
--  SELECT 'J2000     0 240.0 38.0' ,   dbo.fHTM_Lookup('J2000 0 240.0 38.0'     )as HTM_ID --   =             13
--  SELECT 'J2000     5 240.0 38.0' ,   dbo.fHTM_Lookup('J2000 5 240.0 38.0'     )as HTM_ID --   =          14248
--  SELECT 'J2000     20 240.0 38.0' ,  dbo.fHTM_Lookup('J2000 20 240.0 38.0'     )as HTM_ID -- = 15299371974207
--  SELECT 'J2000     6 41.4 47.9' ,    dbo.fHTM_Lookup('J2000 6 41.4 47.9'     )as HTM_ID  -- 65351
--  SELECT 'CARTESIAN 9 1.0 1.0 1.0',   dbo.fHTM_Lookup('CARTESIAN 9 1.0 1.0 1.0 ') as HTM_ID --4194303
--  SELECT 'CARTESIAN 9 1.0 1.0 1.0',   dbo.fHTM_Lookup('CARTESIAN 1 1.0 1.0 1.0 ') as HTM_ID  -- 63

-- some error cases
--  SELECT 'J2001     20  240.0 38.0' , dbo.fHTM_Lookup_ErrorMessage('J2001 20 240.0 38.0'     )
--  SELECT 'J2000      100 240.0 38.0', dbo.fHTM_Lookup_ErrorMessage('J2000 100 240.0 38.0'     )
--  SELECT 'J2000     20 240.0' ,       dbo.fHTM_Lookup_ErrorMessage('J2000 XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX'     )
--  SELECT 'J2000     20 240.0' ,       dbo.fHTM_Lookup_ErrorMessage('J2000 20 240.0'     )


------------------------------------------------------------------
-- some test cases for fHTM
-- SELECT * FROM fHTM_Cover('CIRCLE J2000 3 41.4 47.9 .1 ') -- (1021, 1022)
--   SELECT * FROM fHTM_Cover('CIRCLE J2000 6 41.4 47.9 20 ') -- (65348, 65349), (65351, 65352) 
--   SELECT * FROM fHTM_Cover('CONVEX J2000 6 1 2 3 4 5 6 ') -- 8 cells
--   SELECT * FROM fHTM_Cover('DOMAIN  2 1 1 0 0 .5 1 0 1 0 .5 ')  -- 4 cells.
--   SELECT dbo.fHTM_Cover_ErrorMessage('CIRCLE J2001 3 41.4 47.9 .1 ') 
--   SELECT dbo.fHTM_Cover_ErrorMessage('CONVEX J2000 6 1 2 3 4 5 6 ')
--   SELECT dbo.fHTM_Cover_ErrorMessage('DOMAIN  10 10 1 0 0 .5 1 0 1 0 .5 ')
-- 
--   Create table t(HTM1 bigint,HTM2 bigint)
--   insert into t 
--       select * from fHTM_Cover('CONVEX J2000 6 1 2 3 4 5 6 ')
--   select * from t 
--   drop table t 
--    
--   SELECT * FROM fHTM_Cover('CIRCLE J2000 3 41.4 47.9 .1 ') 
--   GO
select dbo.fHTM_Version()

