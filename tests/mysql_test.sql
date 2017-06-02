
/*!40101 SET NAMES utf8 */;

/*!40101 SET SQL_MODE=''*/;

/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

CREATE DATABASE /*!32312 IF NOT EXISTS*/`mysql_test` /*!40100 DEFAULT CHARACTER SET utf8 */;

USE `mysql_test`;

/*Table structure for table `mysql_test` */

DROP TABLE IF EXISTS `mysql_test`;

CREATE TABLE `mysql_test` (
  `id` int(11) DEFAULT NULL,
  `name` varchar(20) DEFAULT NULL,
  `sex` tinyint(4) DEFAULT NULL,
  `age` int(10) unsigned DEFAULT NULL,
  `diamond` bigint(20) DEFAULT NULL,
  `money` bigint(20) unsigned DEFAULT NULL,
  `ext` blob
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
