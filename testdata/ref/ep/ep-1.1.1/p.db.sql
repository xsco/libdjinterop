PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE PerformanceData ( [id] INTEGER, [isAnalyzed] NUMERIC, [isRendered] NUMERIC, [trackData] BLOB, [highResolutionWaveFormData] BLOB, [overviewWaveFormData] BLOB, [beatData] BLOB, [quickCues] BLOB, [loops] BLOB, [hasSeratoValues] NUMERIC, [hasRekordboxValues] NUMERIC, PRIMARY KEY ( [id] ) );
CREATE TABLE Information ( [id] INTEGER, [uuid] TEXT, [schemaVersionMajor] INTEGER, [schemaVersionMinor] INTEGER, [schemaVersionPatch] INTEGER, [currentPlayedIndiciator] INTEGER, [lastRekordBoxLibraryImportReadCounter] INTEGER, PRIMARY KEY ( [id] ) );
INSERT INTO Information VALUES(1,'cd1a3527-e04d-4898-b49c-56e8423ceedb',1,9,1,0,0);
CREATE INDEX index_PerformanceData_id ON PerformanceData ( id );
CREATE INDEX index_Information_id ON Information ( id );
COMMIT;
