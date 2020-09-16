PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE PerformanceData ( [id] INTEGER, [isAnalyzed] NUMERIC, [isRendered] NUMERIC, [trackData] BLOB, [highResolutionWaveFormData] BLOB, [overviewWaveFormData] BLOB, [beatData] BLOB, [quickCues] BLOB, [loops] BLOB, [hasSeratoValues] NUMERIC, [hasRekordboxValues] NUMERIC, [hasTraktorValues] NUMERIC, PRIMARY KEY ( [id] ) );
CREATE TABLE Information ( [id] INTEGER, [uuid] TEXT, [schemaVersionMajor] INTEGER, [schemaVersionMinor] INTEGER, [schemaVersionPatch] INTEGER, [currentPlayedIndiciator] INTEGER, [lastRekordBoxLibraryImportReadCounter] INTEGER, PRIMARY KEY ( [id] ) );
INSERT INTO Information VALUES(1,'bb169dde-43b5-4084-824c-1dcebe62e375',1,13,2,0,0);
CREATE INDEX index_PerformanceData_id ON PerformanceData ( id );
CREATE INDEX index_Information_id ON Information ( id );
COMMIT;
