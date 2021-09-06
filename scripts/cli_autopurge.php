<?php
/**
 * Account autopurge - must be added on a cron job!!!
 *
 * # m h d m w command
 *   0 * * * * php -q /path_to_bardcanvas_install/accounts/scripts/cli_autopurge.php > /dev/null
 * 
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @param string "key"
 * @param string "value"
 * 
 * @return string "OK" on success | error message
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_tools\cli_colortags;

set_time_limit(60*120);
chdir(__DIR__);

include "../../config.php";
include "../../includes/bootstrap.inc";

$current_module = $modules["accounts"];

$boundary  = date("Y-m-d H:i:s", strtotime("now - 60 minutes"));
$dev_res   = $database->query("select * from account_devices where state = 'unregistered' and creation_date < '$boundary'");
$dev_count = $database->num_rows($dev_res);
if( $dev_count > 0 )
{
    $start = time();
    $now   = date("Y-m-d H:i:s");
    cli_colortags::write("<cyan>[$now] Starting deletion of $dev_count unregistered devices...</cyan>\n");
    while($row = $database->fetch_object($dev_res))
    {
        $acc  = new account($row->id_account);
        if( ! $acc->_exists )
        {
            $acc->user_name    = "N/A";
            $acc->display_name = "Not found";
        }
        $ago  = time_elapsed_string($row->creation_date);
        $used = time_elapsed_string($row->last_activity);
        cli_colortags::write(
            "<cyan> • {$acc->user_name} ({$acc->display_name})</cyan> " .
            "<light_cyan>[{$row->device_header}]</light_cyan> " .
            "<brown>created $ago</brown> <yellow>used $used</yellow> " .
            "<light_cyan>deleted.</light_cyan>\n"
        );
        
        $database->exec("delete from account_devices where id_device = '$row->id_device'");
    }
    $seconds = time() - $start;
    cli_colortags::write("<cyan>Finished in $seconds seconds</cyan>.\n\n");
}

$repository = new accounts_repository();
$boundary   = date("Y-m-d H:i:s", strtotime("now - 12 hours"));
$rows       = $repository->find(array("state = 'new'", "creation_date < '$boundary'"), 0, 0, "creation_date asc");
if( count($rows) > 0 )
{
    $start = time();
    $now   = date("Y-m-d H:i:s");
    $count = count($rows);
    cli_colortags::write("<cyan>[$now] Starting deletion of $count unconfirmed accounts...</cyan>\n");
    foreach($rows as $row)
    {
        $ago = time_elapsed_string($row->creation_date);
        cli_colortags::write(
            "<light_cyan> • {$row->user_name} ({$row->display_name}) </light_cyan>" .
            "<yellow>created $ago</yellow>" . 
            "<light_cyan> deleted.</light_cyan>\n"
        );
        $repository->delete($row->id_account);
    }
    $seconds = time() - $start;
    cli_colortags::write("<cyan>Finished in $seconds seconds</cyan>.\n\n");
}
