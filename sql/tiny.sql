use gyuribase
go
--- SELECT * FROM dbo.fHTM_Cover('DOMAIN  2 1  1   0   0  .5  1 0 1 0 .5 ')
-- SELECT * FROM dbo.fHTM_Cover('DOMAIN 2 1 1 0 0 0.99 1 1 0 0 0.99')
SELECT * FROM dbo.fHTM_Cover('DOMAIN 20 1 1 1 0 0 0.99')
--- SELECT * FROM dbo.fHTM_Cover('DOMAIN  1 1 0.0 0.0 1.0 0.9999') ---
--- Should yield :
--- 13469017440256 13469285875711
--- 14568529068032 14568797503487
--- 15668040695808 15668309131263
--- 16767552323584 16767820759039
---SELECT 'J2000     0 240.0 38.0' ,
---	dbo.fHTM_Lookup('J2000 0 240.0 38.0'     )as HTM_ID
---- should yield 13
---SELECT 'J2000     20 240.0 38.0' ,
---	dbo.fHTM_Lookup('J2000 20 240.0 38.0'     )as HTM_ID 
-- should be 15299371974207
