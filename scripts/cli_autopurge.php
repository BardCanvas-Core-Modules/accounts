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

use hng2_base\accounts_repository;
use hng2_tools\cli_colortags;

set_time_limit(60*120);
chdir(__DIR__);

include "../../config.php";
include "../../includes/bootstrap.inc";

$repository = new accounts_repository();
$boundary   = date("Y-m-d H:i:s", strtotime("now - 12 hours"));
$rows       = $repository->find(array("state = 'new'", "creation_date < '$boundary'"), 0, 0, "creation_date asc");

if( count($rows) == 0 ) exit;

$start = time();
$now   = date("Y-m-d H:i:s");
$count = count($rows);
cli_colortags::write("<cyan>[$now] Starting deletion of $count records...</cyan>\n");
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
