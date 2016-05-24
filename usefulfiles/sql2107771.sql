-- phpMyAdmin SQL Dump
-- version 3.5.5
-- http://www.phpmyadmin.net
--
-- Host: sql2.freemysqlhosting.net
-- Loomise aeg: Mai 24, 2016 kell 06:44 PM
-- Serveri versioon: 5.5.49-0ubuntu0.12.04.1
-- PHP versioon: 5.3.28

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Andmebaas: `sql2107771`
--

-- --------------------------------------------------------

--
-- Tabeli struktuur tabelile `games`
--

CREATE TABLE IF NOT EXISTS `games` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `status` varchar(150) NOT NULL DEFAULT 'open',
  `CT` varchar(150) NOT NULL,
  `T` varchar(150) NOT NULL,
  `map` varchar(150) NOT NULL,
  `gstatus` varchar(150) NOT NULL DEFAULT 'NULL',
  `server_ID` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

-- --------------------------------------------------------

--
-- Tabeli struktuur tabelile `maplist`
--

CREATE TABLE IF NOT EXISTS `maplist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `mapname` varchar(150) CHARACTER SET utf32 COLLATE utf32_swedish_ci NOT NULL,
  `usable` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=3 ;

--
-- Andmete tõmmistamine tabelile `maplist`
--

INSERT INTO `maplist` (`id`, `mapname`, `usable`) VALUES
(1, 'de_nuke', 1),
(2, 'de_dust2', 1);

-- --------------------------------------------------------

--
-- Tabeli struktuur tabelile `servers`
--

CREATE TABLE IF NOT EXISTS `servers` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ip` varchar(150) CHARACTER SET ucs2 COLLATE ucs2_swedish_ci NOT NULL,
  `port` int(11) NOT NULL,
  `rcon` varchar(150) CHARACTER SET ucs2 COLLATE ucs2_swedish_ci NOT NULL,
  `usable` varchar(150) CHARACTER SET ucs2 COLLATE ucs2_swedish_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

--
-- Andmete tõmmistamine tabelile `servers`
--

INSERT INTO `servers` (`id`, `ip`, `port`, `rcon`, `usable`) VALUES
(1, '127.0.0.1', 27015, 'pede', 'no');

-- --------------------------------------------------------

--
-- Tabeli struktuur tabelile `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(150) NOT NULL,
  `auth` varchar(150) NOT NULL,
  `rank` int(11) NOT NULL,
  `rating` int(11) NOT NULL,
  `wins` int(11) NOT NULL,
  `loses` int(11) NOT NULL,
  `steamid` int(11) NOT NULL,
  `authkey` varchar(150) NOT NULL,
  `lastgame` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=16 ;

--
-- Andmete tõmmistamine tabelile `users`
--

INSERT INTO `users` (`id`, `name`, `auth`, `rank`, `rating`, `wins`, `loses`, `steamid`, `authkey`, `lastgame`) VALUES
(1, 'sYCH3L', 'Enari', 5, 500, 5, 6, 0, '0', 1),
(2, 'testikasutaja', 'testikasutaja', 0, 0, 0, 0, 0, '', 0),
(3, 'pidr', 'pidr', 5, 0, 0, 0, 0, '', 1),
(4, 'padr', 'padr', 1, 0, 0, 0, 0, '', 2),
(5, 'player1', 'player1', 1, 0, 0, 0, 0, '', 1),
(6, 'player2', 'player2', 1, 0, 0, 0, 0, '', 1),
(7, 'player3', 'player3', 1, 0, 0, 0, 0, '', 1),
(8, 'player4', 'player4', 1, 0, 0, 0, 0, '', 1),
(9, 'player5', 'player5', 1, 0, 0, 0, 0, '', 1),
(10, 'player6', 'player6', 1, 0, 0, 0, 0, '', 1),
(11, 'player7', 'player7', 1, 0, 0, 0, 0, '', 1),
(12, 'player8', 'player8', 1, 0, 0, 0, 0, '', 1),
(14, 'tyrbel', 'tyrbel', 1, 0, 0, 0, 0, '', 1),
(15, 'player9', 'player9', 1, 0, 0, 0, 0, '', 0);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
