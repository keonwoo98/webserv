#!/opt/homebrew/bin/php-cgi

<!DOCTYPE html>
<html>
<head>
    <title>구구단 출력</title>
</head>
<body>
    <h1>원하는 구구단 출력하기</h1>

    <form method="POST" action="./gugu.php">
        보고 싶은 단을 입력하고 제출을 누르세요. <br/>
        <input type="text" name="input"/>
        <input type="submit">
    </form>

    <form method="GET" action="./gugu.php">
        GET Test <br/>
        <input type="text" name="number"/>
        <input type="submit"/>
    </form>

    <?php
    if ($_POST['input'] > 0) {
        $num = (int)($_POST['input']);
        echo $num."단 입니다.<br/>";
        for ($j = 1 ; $j <= 9 ; $j++) {
            echo $num." X ".$j." = ".($num * $j)."<br/>";
        }
        echo "<br/>";
    }

    if ($_GET['number'] > 0) {
        echo "GET Number : " . $_GET['number']. "<br/>";
    }
    ?>
</body>
</html>
