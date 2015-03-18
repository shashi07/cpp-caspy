CREATE DATABASE IF NOT EXISTS caspy;
CREATE USER 'caspy'@'localhost' IDENTIFIED BY 'caspy';
USE caspy;
grant all on `caspy`.* to 'caspy'@'localhost' identified by 'caspy';
Drop table if exists dirs;
Drop table if exists filelist;
Drop table if exists hashes;
Drop table if exists fileBlocks;

CREATE TABLE `dirs` (
  `dir_id` int(11) NOT NULL AUTO_INCREMENT,
  `dir_name` varchar(100) DEFAULT NULL,
  `parent_dir` varchar(100) DEFAULT NULL,
  `username` varchar(25) DEFAULT NULL,
  PRIMARY KEY (`dir_id`)
);

CREATE TABLE `hashes` (
  `hash_id` int(11) NOT NULL AUTO_INCREMENT,
  `hash` varchar(40) NOT NULL,
  `size` int(11) NOT NULL,
  PRIMARY KEY (`hash_id`)
);

CREATE TABLE `filelist` (
  `f_id` int(11) NOT NULL AUTO_INCREMENT,
  `filename` varchar(25) NOT NULL,
  `no_of_blocks` int(11) DEFAULT NULL,
  `dir_id` int(11) NOT NULL,
  PRIMARY KEY (`f_id`),
  UNIQUE KEY `unique_index` (`filename`,`dir_id`),
  KEY `dir_id` (`dir_id`),
  CONSTRAINT `filelist_ibfk_1` FOREIGN KEY (`dir_id`) REFERENCES `dirs` (`dir_id`)
);

CREATE TABLE `fileBlocks` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `block_no` int(11) NOT NULL,
  `f_id` int(11) NOT NULL,
  `hash_id` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `unique_index_block` (`block_no`,`f_id`),
  KEY `file_fk` (`f_id`),
  KEY `hash_fk` (`hash_id`),
  CONSTRAINT `file_fk` FOREIGN KEY (`f_id`) REFERENCES `filelist` (`f_id`),
  CONSTRAINT `hash_fk` FOREIGN KEY (`hash_id`) REFERENCES `hashes` (`hash_id`)
);
