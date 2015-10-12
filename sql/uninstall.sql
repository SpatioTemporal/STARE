-- The extended stored procedures are in the master database
-- but the t-sql functions are in the database used. This example
-- installs and uninstalls from a toy database called
-- 'gyuribase'. You must edit the string to fit your own site
--
USE master
GO

sp_dropextendedproc 'xp_HTM_Cover'
GO

sp_dropextendedproc 'xp_HTM_Lookup'
GO

USE gyuribase
IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_Lookup')
	DROP FUNCTION fHTM_Lookup 
GO
IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_Lookup_ErrorMessage')
	DROP FUNCTION fHTM_Lookup_ErrorMessage 
GO

IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_Cover')
	DROP FUNCTION fHTM_Cover
GO

IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_Cover_ErrorMessage')
	DROP FUNCTION fHTM_Cover_ErrorMessage 
GO

IF EXISTS (SELECT * 
	   FROM   sysobjects 
	   WHERE  name = N'fHTM_To_String')
	DROP FUNCTION fHTM_To_String
GO
--
