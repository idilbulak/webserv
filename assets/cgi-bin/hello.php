<html>
<head>
    <title>Visitor Name</title>
</head>
<body>
<h1>What is your name?</h1>
<form method="post" action="">
    First Name: <input type="text" name="first_name"><br>
    Last Name: <input type="text" name="last_name"><br>
    <input type="submit" value="Submit">
</form>
</body>
</html>

<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $first_name = $_POST['first_name'];
    $last_name = $_POST['last_name'];
    echo "Hello, $first_name $last_name!";
}
?>
