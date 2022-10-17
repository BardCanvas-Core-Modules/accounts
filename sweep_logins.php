<?php
/**
 * Account logins cleanup
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

include "../config.php";
include "../includes/bootstrap.inc";
header("Content-Type: text/plain; charset=UTF-8");

set_time_limit(60 * 10);

if( ! $account->has_admin_rights_to_module("accounts") ) throw_fake_401();

$mem_key = "@accounts:sweeping";
$mem_ttl = 60 * 5;

if( $mem_cache->get($mem_key) ) die("Sweeper already running. Please give it a few minutes to end and try again.");

$mem_cache->set($mem_key, "true", 0, $mem_ttl);
$res = $database->query("select distinct id_account from account_logins");
$all = $database->num_rows($res);
$idx = 1;

echo "Starting seeep on $all accounts.\n";

while($row = $database->fetch_object($res))
{
    $ares = $database->query("select user_name, display_name from account where id_account = '{$row->id_account}'");
    $arow = $database->fetch_object($ares);
    
    $res2 = $database->query("select login_date from account_logins where id_account = '{$row->id_account}' order by login_date desc limit 1");
    $row2 = $database->fetch_object($res2);
    
    $count = $database->exec("delete from account_logins where id_account = '$row->id_account' and login_date < '$row2->login_date'");
    if( $count > 0 ) echo "[$idx/$all] Deleted $count rows prior {$row2->login_date} for account #{$row->id_account} ({$arow->display_name}, @{$arow->user_name})\n";
    $idx++;
}

echo "Sweep finished.\n";
$mem_cache->delete($mem_key);
