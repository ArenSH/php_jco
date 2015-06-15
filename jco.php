<?php
ini_set("memory_limit", "512M");

$data = range(1, 500000);

$t1 = microtime(true);
$m1 = memory_get_usage();

$jar = new \JCO\DArray(500000, 0);
foreach($data as $index => &$val) {
	$jar[$index] = $val * 3;
}

echo "JCO\Darray" . PHP_EOL;
echo "TIME: " . (microtime(true) - $t1) . PHP_EOL;
echo "MEMORY: " . ((memory_get_usage() - $m1)/1048576) . PHP_EOL;
gc_collect_cycles();


$t1 = microtime(true);
$m1 = memory_get_usage();
$ar = [];

foreach($data as $index => &$val) {
	$ar[$index] = $val * 3;
}

echo "AR" . PHP_EOL;
echo "TIME: " . (microtime(true) - $t1) . PHP_EOL;
echo "MEMORY: " . ((memory_get_usage() - $m1)/1048576) . PHP_EOL;
gc_collect_cycles();



foreach($jar as $val) {
	if(($val % 100000) == 0) {
		echo $val . PHP_EOL;
	}
}
