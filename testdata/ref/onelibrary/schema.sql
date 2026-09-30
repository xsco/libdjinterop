-- The schema a real OneLibrary export carries, abbreviated to the tables that
-- libdjinterop reads.
--
-- Nothing in a real export is declared NOT NULL, and no foreign key is
-- enforced, so a fixture can leave any column unset.
--
-- The published description of the format, which the pyrekordbox project
-- documents as Device Library Plus, lists neither content.djPlayCount nor
-- album.image_id, and gives playlist_content and property primary keys that an
-- export does not carry.  What a device holds is what is written here.

CREATE TABLE content(content_id integer primary key, title varchar, titleForSearch varchar, subtitle varchar, bpmx100 integer, length integer, trackNo integer, discNo integer, artist_id_artist integer, artist_id_remixer integer, artist_id_originalArtist integer, artist_id_composer integer, artist_id_lyricist integer, album_id integer, genre_id integer, label_id integer, key_id integer, color_id integer, image_id integer, djComment varchar, rating integer, releaseYear integer, releaseDate varchar, dateCreated varchar, dateAdded varchar, path varchar, fileName varchar, fileSize integer, fileType integer, bitrate integer, bitDepth integer, samplingRate integer, isrc varchar, djPlayCount integer, isHotCueAutoLoadOn integer, isKuvoDeliverStatusOn integer, kuvoDeliveryComment varchar, masterDbId integer, masterContentId integer, analysisDataFilePath varchar, analysedBits integer, contentLink integer, hasModified integer, cueUpdateCount integer, analysisDataUpdateCount integer, informationUpdateCount integer);

CREATE TABLE artist(artist_id integer primary key, name varchar, nameForSearch varchar);

CREATE TABLE album(album_id integer primary key, name varchar, artist_id integer, image_id integer, isComplation integer, nameForSearch varchar);

CREATE TABLE genre(genre_id integer primary key, name varchar);

CREATE TABLE label(label_id integer primary key, name varchar);

CREATE TABLE "key"(key_id integer primary key, name varchar);

CREATE TABLE playlist(playlist_id integer primary key, sequenceNo integer, name varchar, image_id integer, attribute integer, playlist_id_parent integer);

CREATE TABLE playlist_content(playlist_id integer, content_id integer, sequenceNo integer);

CREATE TABLE property(deviceName varchar, dbVersion varchar, numberOfContents integer, createdDate varchar, backGroundColorType integer, myTagMasterDBID integer);

