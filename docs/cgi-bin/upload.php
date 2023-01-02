<?php
$base_dir = getenv('UPLOAD_PATH');
$source = $_FILES['profile']['tmp_name'];
$dest = $base_dir.basename($_FILES['profile']['name']);
move_uploaded_file($source, $dest);
$source = $_FILES['background']['tmp_name'];
$dest = $base_dir.basename($_FILES['background']['name']);
move_uploaded_file($source, $dest);
$source = $_FILES['please']['tmp_name'];
$dest = $base_dir.basename($_FILES['please']['name']);
move_uploaded_file($source, $dest);
?>

<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title></title>
</head>
<body>
	<img src="<?=$base_dir.$_FILES['profile']['name']?>" alt="" />
	<img src="<?=$base_dir.$_FILES['background']['name']?>" alt="" />
	<img src="<?=$base_dir.$_FILES['please']['name']?>" alt="" />
</body>
</html>
