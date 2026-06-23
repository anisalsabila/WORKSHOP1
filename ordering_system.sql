-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 21, 2025 at 04:28 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `ordering_system`
--

-- --------------------------------------------------------

--
-- Table structure for table `cart`
--

CREATE TABLE `cart` (
  `CustomerID` varchar(255) NOT NULL,
  `ItemID` varchar(255) NOT NULL,
  `OrderQuantity` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `cart`
--

INSERT INTO `cart` (`CustomerID`, `ItemID`, `OrderQuantity`) VALUES
('chartati', 'm45', 1);

-- --------------------------------------------------------

--
-- Table structure for table `customer`
--

CREATE TABLE `customer` (
  `CustomerID` varchar(30) NOT NULL,
  `CustName` varchar(60) NOT NULL,
  `CustTelNum` varchar(12) NOT NULL,
  `CustPassword` varchar(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `customer`
--

INSERT INTO `customer` (`CustomerID`, `CustName`, `CustTelNum`, `CustPassword`) VALUES
('Cbella', 'Isabell', '60199839022', '!!Bella19'),
('Cdidi', 'Didi sofia', '60199343244', '!!Didi19'),
('Cdina', 'Dina', '60155893023', '!!Dina19'),
('Cedzuan', 'Edzuan', '60199357255', '!!Edzuan19'),
('Cfaz', 'fazizi', '60199232133', '!!Fazi19'),
('Chartati', 'Ani Hartati', '60143839248', '!!Hartati19'),
('Cipah', 'sharifah', '178989453', '!!Sharifah19'),
('Cnaja', 'Naja', '60199343244', '!!Naja19'),
('Cnajwa', 'Najwa', '60199345433', '!!Najwa19'),
('Cputri', 'Putri', '60199878544', '!!Putri19'),
('Cqiqi', 'Qirana', '60199343299', '!!Qiran19');

-- --------------------------------------------------------

--
-- Table structure for table `feedback`
--

CREATE TABLE `feedback` (
  `FeedbackID` int(11) NOT NULL,
  `CustomerID` varchar(50) NOT NULL,
  `OrderID` int(11) NOT NULL,
  `Rating` int(11) NOT NULL CHECK (`Rating` between 1 and 5),
  `Comments` text DEFAULT NULL,
  `FeedbackDate` timestamp NOT NULL DEFAULT current_timestamp()
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `feedback`
--

INSERT INTO `feedback` (`FeedbackID`, `CustomerID`, `OrderID`, `Rating`, `Comments`, `FeedbackDate`) VALUES
(1, 'chartati', 80, 5, 'this is so yummyy', '2025-01-11 16:01:59'),
(2, 'cdina', 84, 5, 'food was amazing. love the matcha strawberry!', '2025-01-15 20:39:22'),
(4, 'cnajwa', 86, 4, 'So good', '2025-01-20 15:44:02'),
(5, 'cdidi', 87, 5, 'The food is good', '2025-01-21 00:34:17');

-- --------------------------------------------------------

--
-- Table structure for table `menu item`
--

CREATE TABLE `menu item` (
  `ItemID` varchar(5) NOT NULL,
  `ItemName` varchar(50) NOT NULL,
  `ItemPrice` decimal(10,2) DEFAULT NULL,
  `ItemCategory` varchar(10) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `menu item`
--

INSERT INTO `menu item` (`ItemID`, `ItemName`, `ItemPrice`, `ItemCategory`) VALUES
('M10', 'Mix Berries Refresher', 8.00, 'Drink'),
('M11', 'Chicken Breast Sandwich', 6.00, 'Food'),
('M12', 'Egg Tart', 3.00, 'Food'),
('M13', 'Plain Croissant', 4.00, 'Food'),
('M14', 'Sugar Coated Donut', 3.00, 'Food'),
('M15', 'Carbonara Spaghetti', 18.00, 'Food'),
('M16', 'Bolognese Spaghetti', 19.00, 'Food'),
('M17', 'Cheesy Fries', 6.00, 'Food'),
('M18', 'Fries', 4.00, 'Food'),
('M19', 'Soft Cookies', 6.00, 'Food'),
('M2', 'Flat White', 6.00, 'Drink'),
('M20', 'Fluffy Milky Bun', 10.00, 'Food'),
('M21', 'Apple', 2.00, 'Food'),
('M22', 'Pepperoni Pizza', 24.00, 'Food'),
('M23', 'Chocolate Cake', 10.00, 'Cake'),
('M24', 'Vanilla Cake', 9.00, 'Cake'),
('M25', 'Tiramisu Cake', 10.00, 'Cake'),
('M26', 'Earl Grey Lavender Cake', 16.00, 'Cake'),
('M27', 'Matcha White Chocolate Cake', 15.00, 'Cake'),
('M28', 'Pistachio Rose Cake', 14.00, 'Cake'),
('M29', 'Black Sesame Honey Cake', 12.00, 'Cake'),
('M3', 'Cold Brew', 4.00, 'Drink'),
('M30', 'Strawberry Shortcake Bliss', 17.00, 'Cake'),
('M31', 'Crispy Caesar Delight', 12.50, 'Salad'),
('M32', 'Mediterranean Quinoa Salad', 13.00, 'Salad'),
('M33', 'Avocado Citrus Salad', 14.00, 'Salad'),
('M34', 'Greek Feta & Olive Salad', 13.50, 'Salad'),
('M35', 'Roasted Beetroot & Goat Cheese Salad', 15.00, 'Salad'),
('M36', 'Pistachio Dream Gelato', 6.50, 'Gelato'),
('M37', 'Strawberry Vanilla Swirl', 6.00, 'Gelato'),
('M38', 'Rich Chocolate Hazelnut', 7.00, 'Gelato'),
('M39', 'Lemon Sour Sorbeto', 9.00, 'Gelato'),
('M4', 'Long Black', 5.00, 'Drink'),
('M40', 'Salted Caramel Bliss', 6.50, 'Gelato'),
('M41', 'Matcha Green Tea Gelato', 7.00, 'Gelato'),
('M42', 'Tiramisu Gelato', 6.50, 'Gelato'),
('M43', 'Mango Passion Fruit Gelato', 6.00, 'Gelato'),
('M44', 'Coconut Milk Gelato', 6.50, 'Gelato'),
('M45', 'Blueberry Cheesecake Gelato', 7.00, 'Gelato'),
('M46', 'Hearty Chicken & Vegetable Soup', 10.00, 'Soup'),
('M47', 'Spicy Butternut Squash Soup', 11.00, 'Soup'),
('M48', 'Classic French Onion Soup', 12.00, 'Soup'),
('M49', 'Lentil & Spinach Soup', 9.00, 'Soup'),
('M5', 'Matcha Latte', 7.00, 'Drink'),
('M50', 'Minestrone Soup with Pesto', 10.50, 'Soup'),
('M51', 'Wild Mushroom & Thyme Soup', 11.50, 'Soup'),
('M52', 'Roasted Carrot & Ginger Soup', 10.00, 'Soup'),
('M53', 'Chicken soup', 12.00, 'Soup'),
('M54', 'Mineral Water', 4.00, 'Drink'),
('M55', 'Wild Rasberry Gelato', 9.50, 'Gelato'),
('M56', 'Muffin', 4.00, 'Cake'),
('M57', 'Chocolate cake', 4.00, 'Cake'),
('M6', 'Strawberry Matcha Latte', 10.00, 'Drink'),
('M7', 'Ice Americano', 2.00, 'Drink'),
('M8', 'Spanish Latte', 6.00, 'Drink'),
('M9', 'Ice Caramel Machiato', 8.00, 'Drink');

-- --------------------------------------------------------

--
-- Table structure for table `order item`
--

CREATE TABLE `order item` (
  `OrderID` int(5) NOT NULL,
  `ItemID` varchar(5) NOT NULL,
  `ItemName` varchar(255) NOT NULL,
  `OrderQuantity` int(5) NOT NULL,
  `OrderPrice` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `order item`
--

INSERT INTO `order item` (`OrderID`, `ItemID`, `ItemName`, `OrderQuantity`, `OrderPrice`) VALUES
(7, 'm1', '', 2, 4),
(7, 'm15', '', 2, 18),
(8, 'm10', '', 1, 8),
(8, 'm22', '', 2, 24),
(8, 'm3', '', 1, 4),
(9, 'm1', '', 3, 4),
(10, 'm12', '', 1, 3),
(10, 'm17', '', 1, 6),
(10, 'm22', '', 1, 24),
(10, 'm5', '', 1, 7),
(11, 'M12', '', 2, 3),
(16, 'M1', '', 3, 10),
(16, 'M1', 'Jasmine Tea', 3, 4),
(16, 'M5', 'Matcha Latte', 2, 7),
(16, 'M10', 'Mix Berries Refresher', 1, 8),
(16, 'M14', 'Sugar Coated Donut', 4, 3),
(16, 'M17', 'Cheesy Fries', 2, 6),
(16, 'M24', 'Vanilla Cake', 1, 9),
(16, 'M30', 'Strawberry Shortcake Bliss', 1, 17),
(16, 'M41', 'Matcha Green Tea Gelato', 3, 7),
(17, 'M18', '', 2, 4),
(17, 'M23', '', 1, 10),
(17, 'M35', '', 1, 15),
(18, 'M5', '', 2, 7),
(18, 'M47', '', 1, 11),
(18, 'M50', '', 1, 11),
(19, 'M3', '', 3, 4),
(19, 'M29', '', 1, 12),
(19, 'M40', '', 1, 7),
(20, 'M2', '', 2, 6),
(20, 'M12', '', 1, 3),
(20, 'M45', '', 1, 7),
(21, 'M16', 'Bolognese Spaghetti', 2, 19),
(21, 'M6', 'Strawberry Matcha Latte', 1, 10),
(21, 'M4', 'Long Black', 1, 5),
(22, 'M10', 'Mix Berries Refresher', 2, 8),
(22, 'M25', 'Tiramisu Cake', 1, 10),
(22, 'M17', 'Cheesy Fries', 1, 6),
(23, 'M9', 'Ice Caramel Machiato', 3, 8),
(23, 'M13', 'Plain Croissant', 1, 4),
(23, 'M21', 'Apple', 1, 2),
(24, 'M14', 'Sugar Coated Donut', 2, 3),
(24, 'M32', 'Mediterranean Quinoa Salad', 1, 13),
(24, 'M18', 'Fries', 1, 4),
(25, 'M28', 'Pistachio Rose Cake', 2, 14),
(25, 'M19', 'Soft Cookies', 1, 6),
(25, 'M45', 'Blueberry Cheesecake Gelato', 1, 7),
(26, 'M30', 'Strawberry Shortcake Bliss', 2, 17),
(26, 'M39', 'Lemon Sorbetto', 1, 6),
(26, 'M47', 'Spicy Butternut Squash Soup', 1, 11),
(27, 'M18', 'Fries', 2, 4),
(27, 'M15', 'Carbonara Spaghetti', 1, 18),
(27, 'M6', 'Strawberry Matcha Latte', 1, 10),
(28, 'M10', 'Mix Berries Refresher', 1, 8),
(28, 'M30', 'Strawberry Shortcake Bliss', 1, 17),
(28, 'M9', 'Ice Caramel Machiato', 2, 8),
(29, 'M35', 'Roasted Beetroot & Goat Cheese Salad', 1, 15),
(29, 'M27', 'Matcha White Chocolate Cake', 2, 15),
(29, 'M7', 'Ice Americano', 1, 2),
(30, 'M23', 'Chocolate Cake', 2, 10),
(30, 'M36', 'Pistachio Dream Gelato', 2, 7),
(30, 'M3', 'Cold Brew', 1, 4),
(31, 'M24', 'Vanilla Cake', 1, 9),
(31, 'M13', 'Plain Croissant', 2, 4),
(31, 'M40', 'Salted Caramel Bliss', 1, 7),
(32, 'M11', 'Chicken Breast Sandwich', 2, 6),
(32, 'M19', 'Soft Cookies', 2, 6),
(32, 'M41', 'Matcha Green Tea Gelato', 1, 7),
(33, 'M16', 'Bolognese Spaghetti', 2, 19),
(33, 'M20', 'Fluffy Milky Bun', 2, 10),
(33, 'M5', 'Matcha Latte', 1, 7),
(34, 'M48', 'Classic French Onion Soup', 1, 12),
(34, 'M31', 'Crispy Caesar Delight', 1, 13),
(34, 'M2', 'Flat White', 1, 6),
(35, 'M28', 'Pistachio Rose Cake', 1, 14),
(35, 'M39', 'Lemon Sorbetto', 2, 6),
(35, 'M46', 'Hearty Chicken & Vegetable Soup', 1, 10),
(36, 'M37', 'Strawberry Vanilla Swirl', 1, 6),
(36, 'M12', 'Egg Tart', 2, 3),
(36, 'M22', 'Pepperoni Pizza', 1, 24),
(37, 'M44', 'Coconut Milk Gelato', 2, 7),
(37, 'M33', 'Avocado Citrus Salad', 1, 14),
(37, 'M42', 'Tiramisu Gelato', 1, 7),
(38, 'M29', 'Black Sesame Honey Cake', 1, 12),
(38, 'M38', 'Rich Chocolate Hazelnut', 2, 7),
(38, 'M50', 'Minestrone Soup with Pesto', 1, 11),
(39, 'M34', 'Greek Feta & Olive Salad', 2, 14),
(39, 'M8', 'Spanish Latte', 2, 6),
(39, 'M49', 'Lentil & Spinach Soup', 1, 9),
(40, 'M41', 'Matcha Green Tea Gelato', 1, 7),
(40, 'M45', 'Blueberry Cheesecake Gelato', 1, 7),
(40, 'M20', 'Fluffy Milky Bun', 1, 10),
(27, 'M2', 'Flat White', 1, 6),
(27, 'M10', 'Mix Berries Refresher', 2, 8),
(28, 'M11', 'Chicken Breast Sandwich', 1, 6),
(28, 'M17', 'Cheesy Fries', 2, 6),
(29, 'M13', 'Plain Croissant', 3, 4),
(29, 'M4', 'Long Black', 2, 5),
(30, 'M19', 'Soft Cookies', 2, 6),
(30, 'M22', 'Pepperoni Pizza', 1, 24),
(31, 'M7', 'Ice Americano', 3, 2),
(31, 'M27', 'Matcha White Chocolate Cake', 2, 15),
(32, 'M8', 'Spanish Latte', 1, 6),
(32, 'M36', 'Pistachio Dream Gelato', 2, 7),
(33, 'M49', 'Lentil & Spinach Soup', 1, 9),
(33, 'M6', 'Strawberry Matcha Latte', 3, 10),
(34, 'M3', 'Cold Brew', 1, 4),
(34, 'M21', 'Apple', 2, 2),
(35, 'M14', 'Sugar Coated Donut', 3, 3),
(35, 'M23', 'Chocolate Cake', 1, 10),
(36, 'M12', 'Egg Tart', 2, 3),
(36, 'M18', 'Fries', 2, 4),
(37, 'M15', 'Carbonara Spaghetti', 1, 18),
(37, 'M32', 'Mediterranean Quinoa Salad', 1, 13),
(38, 'M37', 'Strawberry Vanilla Swirl', 3, 6),
(38, 'M47', 'Spicy Butternut Squash Soup', 2, 11),
(39, 'M22', 'Pepperoni Pizza', 1, 24),
(39, 'M48', 'Classic French Onion Soup', 2, 12),
(40, 'M5', 'Matcha Latte', 1, 7),
(40, 'M45', 'Blueberry Cheesecake Gelato', 2, 7),
(41, 'M4', 'Long Black', 1, 5),
(41, 'M13', 'Plain Croissant', 2, 4),
(42, 'M16', 'Bolognese Spaghetti', 2, 19),
(42, 'M24', 'Vanilla Cake', 1, 9),
(43, 'M20', 'Fluffy Milky Bun', 3, 10),
(43, 'M28', 'Pistachio Rose Cake', 1, 14),
(44, 'M19', 'Soft Cookies', 1, 6),
(44, 'M30', 'Strawberry Shortcake Bliss', 2, 17),
(45, 'M12', 'Egg Tart', 3, 3),
(45, 'M39', 'Lemon Sorbetto', 1, 6),
(46, 'M5', 'Matcha Latte', 2, 7),
(46, 'M50', 'Minestrone Soup with Pesto', 1, 11),
(47, 'M27', 'Matcha White Chocolate Cake', 1, 15),
(47, 'M33', 'Avocado Citrus Salad', 2, 14),
(48, 'M22', 'Pepperoni Pizza', 2, 24),
(48, 'M40', 'Salted Caramel Bliss', 1, 7),
(49, 'M31', 'Crispy Caesar Delight', 1, 13),
(49, 'M46', 'Hearty Chicken & Vegetable Soup', 2, 10),
(50, 'M3', 'Cold Brew', 2, 4),
(50, 'M15', 'Carbonara Spaghetti', 1, 18),
(50, 'M18', 'Fries', 2, 4),
(51, 'M23', 'Chocolate Cake', 2, 10),
(51, 'M32', 'Mediterranean Quinoa Salad', 1, 13),
(52, 'M7', 'Ice Americano', 1, 2),
(52, 'M17', 'Cheesy Fries', 3, 6),
(52, 'M49', 'Lentil & Spinach Soup', 1, 9),
(53, 'M11', 'Chicken Breast Sandwich', 1, 6),
(53, 'M36', 'Pistachio Dream Gelato', 2, 7),
(54, 'M45', 'Blueberry Cheesecake Gelato', 2, 7),
(54, 'M12', 'Egg Tart', 2, 3),
(55, 'M40', 'Salted Caramel Bliss', 1, 7),
(55, 'M50', 'Minestrone Soup with Pesto', 2, 11),
(56, 'M51', 'Wild Mushroom & Thyme Soup', 2, 12),
(56, 'M5', 'Matcha Latte', 2, 7),
(57, 'M20', 'Fluffy Milky Bun', 1, 10),
(57, 'M46', 'Hearty Chicken & Vegetable Soup', 1, 10),
(58, 'M8', 'Spanish Latte', 2, 6),
(58, 'M21', 'Apple', 4, 2),
(59, 'M6', 'Strawberry Matcha Latte', 1, 10),
(59, 'M29', 'Black Sesame Honey Cake', 2, 12),
(74, 'M1', '', 1, 4),
(74, 'M1', '', 1, 4),
(74, 'M12', '', 2, 3),
(74, 'M23', '', 1, 10),
(76, 'M37', '', 1, 6),
(76, 'M37', '', 1, 6),
(78, 'M37', '', 1, 6),
(79, 'M24', '', 1, 9),
(79, 'M37', '', 1, 6),
(80, 'M31', '', 1, 13),
(81, 'm1', '', 2, 4),
(81, 'm23', '', 2, 10),
(81, 'm31', '', 1, 13),
(83, 'm1', '', 1, 4),
(84, 'M1', '', 1, 4),
(84, 'm10', '', 1, 8),
(84, 'M12', '', 2, 3),
(84, 'M23', '', 1, 10),
(84, 'm33', '', 1, 14),
(84, 'm35', '', 1, 15),
(84, 'm6', '', 1, 10),
(85, 'M23', '', 2, 10),
(86, 'M11', '', 1, 6),
(87, 'M11', '', 2, 6);

-- --------------------------------------------------------

--
-- Table structure for table `orderr`
--

CREATE TABLE `orderr` (
  `OrderID` int(11) NOT NULL,
  `ItemName` varchar(20) NOT NULL,
  `OrderQuantity` varchar(3) NOT NULL,
  `OrderTotalPrice` decimal(10,2) DEFAULT NULL,
  `OrderDate` date NOT NULL,
  `ItemID` varchar(5) NOT NULL,
  `CustomerID` varchar(50) NOT NULL,
  `OrderStatus` varchar(50) NOT NULL DEFAULT 'Pending'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `orderr`
--

INSERT INTO `orderr` (`OrderID`, `ItemName`, `OrderQuantity`, `OrderTotalPrice`, `OrderDate`, `ItemID`, `CustomerID`, `OrderStatus`) VALUES
(10, '', '', 40.00, '2024-12-19', '', 'cipah', 'Paid'),
(16, '', '', 120.00, '2024-10-15', '', 'CAhmad', 'Paid'),
(17, '', '', 45.00, '2024-10-15', '', 'CCarmen', 'Paid'),
(18, '', '', 28.00, '2024-10-15', '', 'CZhao', 'Paid'),
(19, '', '', 36.00, '2024-10-15', '', 'CLee', 'Paid'),
(20, '', '', 38.50, '2024-10-15', '', 'CYvonne', 'Paid'),
(21, '', '', 50.00, '2024-10-15', '', 'CLi', 'Paid'),
(22, '', '', 60.00, '2024-10-15', '', 'CKevin', 'Paid'),
(23, '', '', 40.00, '2024-10-15', '', 'CNina', 'Paid'),
(24, '', '', 32.00, '2024-10-15', '', 'CHai', 'Paid'),
(25, '', '', 45.50, '2024-10-15', '', 'CJack', 'Paid'),
(26, '', '', 55.00, '2024-10-15', '', 'CRyan', 'Paid'),
(27, '', '', 58.00, '2024-10-16', '', 'CNina', 'Paid'),
(28, '', '', 66.00, '2024-10-16', '', 'CKevin', 'Paid'),
(29, '', '', 48.50, '2024-10-16', '', 'CLi', 'Paid'),
(30, '', '', 41.50, '2024-10-16', '', 'CRyan', 'Paid'),
(31, '', '', 33.50, '2024-10-16', '', 'CHai', 'Paid'),
(32, '', '', 55.00, '2024-10-16', '', 'CJack', 'Paid'),
(33, '', '', 52.00, '2024-10-16', '', 'CLi', 'Paid'),
(34, '', '', 61.00, '2024-10-16', '', 'CKevin', 'Paid'),
(35, '', '', 47.00, '2024-10-16', '', 'CNina', 'Paid'),
(36, '', '', 42.00, '2024-10-16', '', 'CHai', 'Paid'),
(37, '', '', 49.00, '2024-10-16', '', 'CJack', 'Paid'),
(38, '', '', 59.00, '2024-10-16', '', 'CRyan', 'Paid'),
(39, '', '', 53.00, '2024-10-16', '', 'CLi', 'Paid'),
(40, '', '', 50.00, '2024-10-16', '', 'CKevin', 'Paid'),
(41, '', '', 39.00, '2024-10-15', '', 'CJames', 'Paid'),
(42, '', '', 25.00, '2024-10-15', '', 'CLeah', 'Paid'),
(43, '', '', 28.50, '2024-10-15', '', 'CJay', 'Paid'),
(44, '', '', 44.00, '2024-10-15', '', 'CSara', 'Paid'),
(45, '', '', 36.50, '2024-10-15', '', 'CYan', 'Paid'),
(46, '', '', 54.00, '2024-10-15', '', 'CNina', 'Paid'),
(47, '', '', 41.00, '2024-10-15', '', 'CJack', 'Paid'),
(48, '', '', 38.50, '2024-10-16', '', 'CEmily', 'Paid'),
(49, '', '', 28.00, '2024-10-16', '', 'CArif', 'Paid'),
(50, '', '', 33.50, '2024-10-16', '', 'CJayne', 'Paid'),
(51, '', '', 29.00, '2024-10-16', '', 'CLeo', 'Paid'),
(52, '', '', 42.00, '2024-10-16', '', 'CEliza', 'Paid'),
(53, '', '', 24.50, '2024-10-16', '', 'CDean', 'Paid'),
(54, '', '', 40.00, '2024-10-16', '', 'CCynthia', 'Paid'),
(55, '', '', 35.00, '2024-12-15', '', 'CLee', 'Paid'),
(56, '', '', 50.00, '2024-12-15', '', 'CLina', 'Paid'),
(57, '', '', 28.50, '2024-12-15', '', 'CKevin', 'Paid'),
(58, '', '', 42.00, '2024-12-15', '', 'CSiti', 'Paid'),
(59, '', '', 29.00, '2024-12-15', '', 'CHassan', 'Paid'),
(60, '', '', 36.00, '2024-12-15', '', 'CCindy', 'Paid'),
(61, '', '', 45.00, '2024-12-15', '', 'CJames', 'Paid'),
(62, '', '', 38.50, '2024-12-15', '', 'CTina', 'Paid'),
(63, '', '', 40.00, '2024-12-15', '', 'CAlex', 'Paid'),
(64, '', '', 58.50, '2024-12-26', '', 'CAdam', 'Paid'),
(65, '', '', 32.00, '2024-12-26', '', 'CLisa', 'Paid'),
(66, '', '', 45.00, '2024-12-26', '', 'CJohn', 'Paid'),
(67, '', '', 29.50, '2024-12-26', '', 'CMay', 'Paid'),
(68, '', '', 38.00, '2024-12-26', '', 'CRyan', 'Paid'),
(69, '', '', 49.50, '2024-12-26', '', 'CMia', 'Paid'),
(70, '', '', 40.00, '2024-12-26', '', 'CMark', 'Paid'),
(71, '', '', 35.00, '2024-12-26', '', 'CTara', 'Paid'),
(72, '', '', 42.00, '2024-12-26', '', 'CLeo', 'Paid'),
(73, '', '', 33.00, '2024-12-26', '', 'CNina', 'Paid'),
(74, '', '', 20.00, '2025-01-07', '', 'cdina', 'Paid'),
(76, '', '', 6.00, '2025-01-11', '', 'cedzuan', 'Canceled'),
(77, '', '', 0.00, '2025-01-11', '', 'cedzuan', 'Canceled'),
(78, '', '', 6.00, '2025-01-11', '', 'cedzuan', 'Paid'),
(79, '', '', 15.00, '2025-01-11', '', 'cedzuan', 'Pending'),
(80, '', '', 12.50, '2025-01-11', '', 'chartati', 'Paid'),
(81, '', '', 32.50, '2025-01-16', '', 'cbella', 'Canceled'),
(82, '', '', 0.00, '2025-01-16', '', 'cbella', 'Paid'),
(83, '', '', 4.00, '2025-01-16', '', 'cbella', 'Pending'),
(84, '', '', 67.00, '2025-01-16', '', 'cdina', 'Paid'),
(85, '', '', 20.00, '2025-01-20', '', 'cputri', 'Paid'),
(86, '', '', 6.00, '2025-01-20', '', 'cnajwa', 'Paid'),
(87, '', '', 12.00, '2025-01-21', '', 'cdidi', 'Paid');

-- --------------------------------------------------------

--
-- Table structure for table `payments`
--

CREATE TABLE `payments` (
  `PaymentID` int(11) NOT NULL,
  `OrderID` int(11) NOT NULL,
  `CustomerID` varchar(255) NOT NULL,
  `PaymentAmount` decimal(10,2) NOT NULL,
  `PaymentDate` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `payments`
--

INSERT INTO `payments` (`PaymentID`, `OrderID`, `CustomerID`, `PaymentAmount`, `PaymentDate`) VALUES
(1, 10, 'cipah', 40.00, '2024-12-19 22:53:26'),
(2, 74, 'cdina', 4.00, '2025-01-07 07:36:21'),
(3, 78, 'cedzuan', 6.00, '2025-01-11 20:26:34'),
(4, 80, 'chartati', 12.50, '2025-01-11 23:57:21'),
(5, 82, 'cbella', 2.00, '2025-01-16 04:29:21'),
(6, 84, 'cdina', 67.00, '2025-01-16 04:37:35'),
(8, 86, 'cnajwa', 6.00, '2025-01-20 23:43:31'),
(9, 87, 'cdidi', 12.00, '2025-01-21 08:33:40');

-- --------------------------------------------------------

--
-- Table structure for table `staff`
--

CREATE TABLE `staff` (
  `StaffID` varchar(30) NOT NULL,
  `StaffPassword` varchar(10) NOT NULL,
  `StaffName` varchar(20) NOT NULL,
  `StaffEmail` varchar(30) NOT NULL,
  `StaffTelNum` varchar(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `staff`
--

INSERT INTO `staff` (`StaffID`, `StaffPassword`, `StaffName`, `StaffEmail`, `StaffTelNum`) VALUES
('Scaca', '!!Caca19', 'Caminaa', 'caca@gmail.com', '60123456789'),
('Sfeesa', '!!Feesa19', 'Nafeesa', 'nafeesa12@gmail.com', '60144903411'),
('Sibi', '!!Ibii19', 'Ibii Naila', 'Ibi@gmail.com', '60188760921'),
('Snailaa', '!!Nailaa19', 'Nailaa', 'Nailaa@gmail.com', '60199454244');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `cart`
--
ALTER TABLE `cart`
  ADD PRIMARY KEY (`CustomerID`,`ItemID`),
  ADD KEY `ItemID` (`ItemID`);

--
-- Indexes for table `customer`
--
ALTER TABLE `customer`
  ADD PRIMARY KEY (`CustomerID`);

--
-- Indexes for table `feedback`
--
ALTER TABLE `feedback`
  ADD PRIMARY KEY (`FeedbackID`),
  ADD KEY `CustomerID` (`CustomerID`),
  ADD KEY `OrderID` (`OrderID`);

--
-- Indexes for table `menu item`
--
ALTER TABLE `menu item`
  ADD PRIMARY KEY (`ItemID`);

--
-- Indexes for table `orderr`
--
ALTER TABLE `orderr`
  ADD PRIMARY KEY (`OrderID`);

--
-- Indexes for table `payments`
--
ALTER TABLE `payments`
  ADD PRIMARY KEY (`PaymentID`),
  ADD KEY `OrderID` (`OrderID`),
  ADD KEY `CustomerID` (`CustomerID`);

--
-- Indexes for table `staff`
--
ALTER TABLE `staff`
  ADD PRIMARY KEY (`StaffID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `feedback`
--
ALTER TABLE `feedback`
  MODIFY `FeedbackID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;

--
-- AUTO_INCREMENT for table `orderr`
--
ALTER TABLE `orderr`
  MODIFY `OrderID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=88;

--
-- AUTO_INCREMENT for table `payments`
--
ALTER TABLE `payments`
  MODIFY `PaymentID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=10;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `cart`
--
ALTER TABLE `cart`
  ADD CONSTRAINT `cart_ibfk_1` FOREIGN KEY (`CustomerID`) REFERENCES `customer` (`CustomerID`),
  ADD CONSTRAINT `cart_ibfk_2` FOREIGN KEY (`ItemID`) REFERENCES `menu item` (`ItemID`);

--
-- Constraints for table `feedback`
--
ALTER TABLE `feedback`
  ADD CONSTRAINT `feedback_ibfk_1` FOREIGN KEY (`CustomerID`) REFERENCES `customer` (`CustomerID`) ON DELETE CASCADE,
  ADD CONSTRAINT `feedback_ibfk_2` FOREIGN KEY (`OrderID`) REFERENCES `orderr` (`OrderID`) ON DELETE CASCADE;

--
-- Constraints for table `payments`
--
ALTER TABLE `payments`
  ADD CONSTRAINT `payments_ibfk_1` FOREIGN KEY (`OrderID`) REFERENCES `orderr` (`OrderID`) ON DELETE CASCADE,
  ADD CONSTRAINT `payments_ibfk_2` FOREIGN KEY (`CustomerID`) REFERENCES `customer` (`CustomerID`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
