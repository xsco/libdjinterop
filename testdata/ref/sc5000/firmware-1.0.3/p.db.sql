PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE PerformanceData ( [id] INTEGER, [isAnalyzed] NUMERIC , [isRendered] NUMERIC , [trackData] BLOB , [highResolutionWaveFormData] BLOB , [overviewWaveFormData] BLOB , [beatData] BLOB , [quickCues] BLOB , [loops] BLOB , [hasSeratoValues] NUMERIC , [hasRekordboxValues] NUMERIC , PRIMARY KEY ( [id] ) );
CREATE TABLE Information ( [id] INTEGER, [uuid] TEXT , [schemaVersionMajor] INTEGER , [schemaVersionMinor] INTEGER , [schemaVersionPatch] INTEGER , [currentPlayedIndiciator] INTEGER , [lastRekordBoxLibraryImportReadCounter] INTEGER , PRIMARY KEY ( [id] ) );
INSERT INTO Information VALUES(1,'ce05bcbc-1402-465d-8822-8a3a0b5a6193',1,7,1,0,0);
CREATE INDEX index_PerformanceData_id ON PerformanceData ( id );
CREATE INDEX index_Information_id ON Information ( id );
COMMIT;
