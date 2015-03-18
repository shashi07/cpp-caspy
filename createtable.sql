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

-- MySQL dump 10.10
--
-- Host: localhost    Database: mysqlfs
-- ------------------------------------------------------
-- Server version	5.0.22-Debian_0ubuntu6.06.2

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `data`
--

DROP TABLE IF EXISTS `data_blocks`;
CREATE TABLE `data_blocks` (
  `inode` bigint(20) NOT NULL,
  `seq` int unsigned not null,
  `data` blob ,
  PRIMARY KEY  (`inode`, `seq`)
) ENGINE=MyISAM DEFAULT CHARSET=binary;

--
-- Table structure for table `inodes`
--

DROP TABLE IF EXISTS `inodes`;
CREATE TABLE `inodes` (
  `inode` bigint(20) NOT NULL,
  `inuse` int(11) NOT NULL default '0',
  `deleted` tinyint(4) NOT NULL default '0',
  `mode` int(11) NOT NULL default '0',
  `uid` int(10) unsigned NOT NULL default '0',
  `gid` int(10) unsigned NOT NULL default '0',
  `atime` int(10) unsigned NOT NULL default '0',
  `mtime` int(10) unsigned NOT NULL default '0',
  `ctime` int(10) unsigned NOT NULL default '0',
  `size` bigint(20) NOT NULL default '0',
  PRIMARY KEY  (`inode`),
  KEY `inode` (`inode`,`inuse`,`deleted`)
) ENGINE=MyISAM DEFAULT CHARSET=binary;

/*!50003 SET @OLD_SQL_MODE=@@SQL_MODE*/;
DELIMITER ;;
/*!50003 SET SESSION SQL_MODE="" */;;
/*!50003 CREATE */ /*!50017 DEFINER=`root`@`localhost` */ /*!50003 TRIGGER `drop_data` AFTER DELETE ON `inodes` FOR EACH ROW BEGIN DELETE FROM data_blocks WHERE inode=OLD.inode; END */;;

DELIMITER ;
/*!50003 SET SESSION SQL_MODE=@OLD_SQL_MODE */;

--
-- Table structure for table `tree`
--

DROP TABLE IF EXISTS `tree`;
CREATE TABLE `tree` (
  `inode` int(10) unsigned NOT NULL auto_increment,
  `parent` int(10) unsigned default NULL,
  `name` varchar(255) NOT NULL,
  UNIQUE KEY `name` (`name`,`parent`),
  KEY `inode` (`inode`),
  KEY `parent` (`parent`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;


