<?php
$source = $_FILES['profile']['tmp_name'];
$dest = "./".basename($_FILES['profile']['name']);
move_uploaded_file($source, $dest);
$source = $_FILES['background']['tmp_name'];
$dest = "./".basename($_FILES['background']['name']);
move_uploaded_file($source, $dest);
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title></title>
</head>
<body>
	<img src="<?=$_FILES['profile']['name']?>" alt="" />
	<img src="<?=$_FILES['background']['name']?>" alt="" />
</body>
</html>
