<?
/**
 * @var config            $config
 * @var settings          $settings
 * @var \SimpleXMLElement $language
 * @var module            $current_module
 * @var array             $errors
 * @var array             $messages
 * @var account           $account
 */

use hng2_base\account;
use hng2_base\accounts_repository;
use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
use hng2_tools\record_browser;

$repository = new accounts_repository();
$browser    = new record_browser("{$config->website_key}_{$current_module->name}");

/**
 * @var int    $offset
 * @var int    $limit
 * @var int    $order
 * @var int    $search_state
 * @var int    $search_level
 * @var string $search_for
 */
#region Nav filters
$built_vars = $browser->build_vars(20, 16);
foreach($built_vars as $key => $val) $$key = $val;

$where_lines = array("account.country = countries.alpha_2");
$search_for  = empty($search_for) ? "" : trim($search_for);
if( ! empty($search_for) )
{
    if( strpos($search_for, ",") !== false )
    {
        $search_for_exploded = explode(",", $search_for);
        $where_line          = "";
        foreach( $search_for_exploded as $this_term )
        {
            $this_term = trim($this_term);
            if( ! empty($this_term) )
            {
                $where_line .= "id_account      =    '" . trim($this_term) . "'   or ";
                $where_line .= "user_name       like '%" . trim($this_term) . "%' or ";
                $where_line .= "display_name    like '%" . trim($this_term) . "%' or ";
                $where_line .= "email           like '%" . trim($this_term) . "%' or ";
                $where_line .= "alt_email       like '%" . trim($this_term) . "%' or ";
                $where_line .= "countries.name  like '%" . trim($this_term) . "%' or ";
            }
        }
        $where_line    = substr($where_line, 0, -4);
        $where_lines[] = $where_line;
        unset($where_line, $search_exploded, $this_term);
    }
    else
    {
        $where_lines[] = "id_account      =    '" . trim($search_for) . "'   or "
            . "user_name       like '%" . trim($search_for) . "%' or "
            . "display_name    like '%" . trim($search_for) . "%' or "
            . "email           like '%" . trim($search_for) . "%' or "
            . "alt_email       like '%" . trim($search_for) . "%' or "
            . "countries.name  like '%" . trim($search_for) . "%'    ";
    }
}

if( empty($search_state) )
{
    if( ! empty($search_level) )
        $where_lines[] = "level = '$search_level'";
}
else
{
    $where_lines[] = "state = '$search_state'";
    if( $search_state == "new" ) $search_level = "";
}

if( ! empty($where_lines) )
{
    $where = "";
    foreach( $where_lines as $this_line ) $where .= " ($this_line) and\n";
    $where        = substr($where, 0, -5);
    $query_filter = "where\n" . $where;
}
#endregion

#region Nav pointers
$queryx = "
    select
        count(account.id_account) as record_count
    from
        account, countries
    $query_filter
";

$resx = $database->query($queryx);
$rowx = $database->fetch_object($resx);
$pagination = $browser->build_pagination($rowx->record_count, $limit, $offset);
#endregion

#region Data grabbing
switch( $order )
{
    case  1: $sqlorder = "user_name asc";                          break;
    case  2: $sqlorder = "user_name desc";                         break;
    case  3: $sqlorder = "display_name asc";                       break;
    case  4: $sqlorder = "display_name desc";                      break;
    case  5: $sqlorder = "country_name asc, display_name asc";     break;
    case  6: $sqlorder = "country_name desc, display_name desc";   break;
    case  7: $sqlorder = "level asc, display_name asc";            break;
    case  8: $sqlorder = "level desc, display_name desc";          break;
    case  9: $sqlorder = "creation_date asc, display_name asc";    break;
    case 10: $sqlorder = "creation_date desc, display_name desc";  break;
    case 13: $sqlorder = "last_update asc, display_name asc";      break;
    case 14: $sqlorder = "last_update desc, display_name desc";    break;
    case 15: $sqlorder = "last_activity asc";                      break;
    case 16: $sqlorder = "last_activity desc";                     break;
    default: $sqlorder = "user_name asc";                          break;
}

$config->globals["accounts:nav/presettings_data"] = array($sqlorder, "");
$current_module->load_extensions("records_browser", "presettings_data");
list($sqlorder, $additional_select_columns) = $config->globals["accounts:nav/presettings_data"];
unset( $config->globals["accounts:nav/presettings_data"] );

$query = "
    select
        account.*,
        countries.name as country_name,
        ( select last_activity
          from account_devices
          where account_devices.id_account = account.id_account
          order by last_activity desc limit 1 ) as last_activity
        $additional_select_columns
    from
        account, countries
    $query_filter
    order by $sqlorder
    limit $offset, $limit
";
$res = $database->query($query);
#endregion
?>

<style type="text/css">
    .principal_addons   { color: #bf0060; padding-left: 18px; margin-top: 10px; margin-bottom: 0; }
    .principal_addons a { color: #bf0060; }
</style>

<h1 style="margin-top: 0;" class="clearfix">
    <button class="pull-right" onclick="$('#filter_form').submit()">
        <span class="fa fa-refresh"></span>
        <?= $language->reload ?>
    </button>
    
    <?= $current_module->language->admin->record_nav->page_title ?>
    
    <? if( $account->level >= config::ADMIN_USER_LEVEL ):
        $style_disable = $settings->get("modules:accounts.register_enabled") != "false" ? ""               : "display: none;";
        $style_enable  = $settings->get("modules:accounts.register_enabled") != "false" ? "display: none;" : ""; ?>
        <span id="registration_toggle_button">
            <button class="toggle_registration" onclick="toggle_registration_mode('off')" style="<?= $style_disable ?>">
                <span class="fa fa-toggle-on"></span>
                <?= $current_module->language->admin->record_nav->disable_register ?>
            </button>
            <button class="toggle_registration" onclick="toggle_registration_mode('on')" style="<?= $style_enable ?>">
                <span class="fa fa-toggle-off"></span>
                <?= $current_module->language->admin->record_nav->enable_register ?>
            </button>
        </span>
    <? endif; ?>
    
    <button onclick="top.location.href='<?= $_SERVER["PHP_SELF"] ?>?mode=show_creation_form&wasuuup=<?= md5(mt_rand(1, 65535)) ?>'">
        <span class="fa fa-plus"></span>
        <?= $current_module->language->admin->record_nav->create ?>
    </button>
</h1>

<? if( count(array_merge($errors, $messages)) > 0 ): ?>
    <div class="framed_content state_ok">
        <? if( count($errors) > 0 ): ?>
            <? foreach( $errors as $item ): ?>
                <span class="fa fa-warning"></span>
                <?= $item ?><br>
            <? endforeach; ?>
        <? endif; ?>
        <? if( count($messages) > 0 ): ?>
            <? foreach( $messages as $item ): ?>
                <span class="fa fa-info-circle"></span>
                <?= $item ?><br>
            <? endforeach; ?>
        <? endif; ?>
    </div>
<? endif; ?>

<? if( empty($_REQUEST["offset"]) && empty($_REQUEST["search_for"]) && empty($_REQUEST["search_level"])
       && $settings->get("modules:accounts.disable_registrations_widget") != "true"): ?>
    <div id="chart" style="margin-bottom: 20px;"></div>
    <script type="text/javascript">
        function show_chart()
        {
            var src = '<?= $config->full_root_path ?>/accounts/scripts/registrations_chart.php?show=true&width=' + $(window).width() + '&height=200&wasuuup=' + parseInt(Math.random() * 1000000000000000);
            $('#chart').html('<img src="' + src + '" style="width: 100%; height: 200px; border: 1px solid black;">');
        }
        
        show_chart();
        $(window).resize(function()
        {
            show_chart();
        });
    </script>
<? endif; ?>

<script type="text/javascript">
    function paginate(value)
    {
        $('form[name="filter_form"] input[name="offset"]').val(value);
        $('form[name="filter_form"]').submit();
    }
    
    function reset_filter()
    {
        document.filter_form.search_for.value           = "";
        document.filter_form.search_level.value         = "";
        document.filter_form.search_state.selectedIndex = 0;
        document.filter_form.limit.value                = "20";
        document.filter_form.order.value                = "16";
        document.filter_form.submit();
    }
</script>

<div class="filtering clearfix">
    <form name="filter_form" id="filter_form" action="<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<? echo mt_rand(1, 65535); ?>"
          method="get">
        
        <input type="hidden" name="mode"         value="set_filter">
        <input type="hidden" name="order"        value="<?= $order ?>">
        <input type="hidden" name="search_level" value="<?= $search_level ?>">
        <input type="hidden" name="offset"       value="0">
        
        <span style="float: right;">
            <?= $browser->get_pagination_button("previous", "paginate", $pagination) ?>
            <?= $language->record_nav->page ?>
            <?= $pagination["this_page_number"]; ?>/<?= $pagination["total_pages"]; ?>
            (<?= $pagination["total_records"]; ?> <?= $language->record_nav->entries ?>)
            <?= $browser->get_pagination_button("next", "paginate", $pagination) ?>
        </span>
        
        <?= $language->record_nav->search ?>
        <input type="text" name="search_for"
               value="<?= htmlspecialchars($search_for) ?>" size="40"
               title="<?= $current_module->language->admin->record_nav->search_title ?>">
        
        <select name="search_state">
            <option <? if( $search_state == "" ) echo "selected"; ?>         value=""        >&lt;<?= $language->record_nav->show_all ?>&gt;</option>
            <option <? if( $search_state == "new" ) echo "selected"; ?>      value="new"     ><?= $current_module->language->admin->record_nav->rec_states->new ?></option>
            <option <? if( $search_state == "enabled" ) echo "selected"; ?>  value="enabled" ><?= $current_module->language->admin->record_nav->rec_states->enabled ?></option>
            <option <? if( $search_state == "disabled" ) echo "selected"; ?> value="disabled"><?= $current_module->language->admin->record_nav->rec_states->disabled ?></option>
        </select>
        
        <?= $language->record_nav->show ?>
        <select name="limit">
            <? foreach(array(5, 10, 20, 30, 40, 50, 100, 200, 300, 400, 500) as $recs): ?>
                <option <? if($limit == $recs) echo "selected" ?>><?= $recs ?></option>
            <? endforeach; ?>
        </select>
        <?= $language->record_nav->recs_per_page ?>
        
        <button type="submit"><?= $language->record_nav->buttons->apply ?></button>
        <button type="submit" onclick="reset_filter();"><?= $language->record_nav->buttons->reset ?></button>
    </form>
    
    <script type="text/javascript">
        function set_search_level(level)
        {
            var $form = $('#filter_form');
            $form.find('input[name="search_level"]').val(level);
            $form.submit();
        }
    </script>
    <div class="records_browser filter_links" style="margin-top: 10px;">
        <?
        $grouped_counts = $repository->get_counts_by_level();
        foreach($config->user_levels_by_level as $level => $name):
            $count = $grouped_counts[$level];
            if( $count == 0 ) continue; ?>
            <span class="framed_content pseudo_link <? if($search_level == $level) echo "state_active"; ?>"
                  onclick="set_search_level('<?= $level ?>')"><?= $name ?> (<?= number_format($count) ?>)</span>
        <? endforeach; ?>
        <span class="framed_content pseudo_link" onclick="set_search_level('')"><?= $language->any ?></span>
    </div>
</div>

<? if( ! $database->num_rows($res) ): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-info-circle"></span>
        <?= $language->record_nav->no_records_found ?>
    </div>
    <? return; ?>
<? endif;  ?>

<div class="table_wrapper">
    <table id="accounts_nav" class="nav_table evened">
        
        <thead>
        <tr>
            <?
            $config->globals["accounts:nav/additional_headers"]
                = $browser->build_table_header($current_module->language->admin->record_nav->column_titles);
            $current_module->load_extensions("records_browser", "additional_headers");
            $headers = $config->globals["accounts:nav/additional_headers"];
            unset( $config->globals["accounts:nav/additional_headers"] );
            foreach( $headers as $this_cell ): ?>
                <th align="<?= $this_cell->xalign ?>" valign="middle" <? if( $this_cell->xnowrap == true ) echo "nowrap "; ?>
                    <? if( ! empty($this_cell->xwidth) ) echo "width='$this_cell->xwidth' "; ?> class="<?= $this_cell->xclass ?>"><?
                    if( $this_cell->sort_asc->enabled == true ) echo "<img
                        src='{$config->full_root_path}/media/icons/order_asc" . ($order == $this_cell->sort_asc->order ? "_this" : "") . ".gif' 
                        align='absmiddle' width='7' height='7'  style='cursor: pointer;'
                        alt='{$this_cell->sort_asc->alt}' title='{$this_cell->sort_asc->alt}'
                        onclick=\"document.filter_form.order.value='{$this_cell->sort_asc->order}'; document.filter_form.submit();\">&nbsp;";
                    ?><?= $this_cell->content ?><?
                    if( $this_cell->sort_desc->enabled == true ) echo "&nbsp;<img
                        src='{$config->full_root_path}/media/icons/order_desc" . ($order == $this_cell->sort_desc->order ? "_this" : "") . ".gif' 
                        align='absmiddle' width='7' height='7'  style='cursor: pointer;'
                        alt='{$this_cell->sort_desc->alt}' title='{$this_cell->sort_desc->alt}'
                        onclick=\"document.filter_form.order.value='{$this_cell->sort_desc->order}'; document.filter_form.submit();\">";
                    ?></th>
            <? endforeach; ?>
        </tr>
        </thead>
        
        <tbody>
        <? while( $row = $database->fetch_object($res) ): ?>
            
            <tr class="<? if( $row->id_account == $account->id_account ) echo "self"; else echo $row->state; ?>"
                id_account="<?= $row->id_account ?>">
                
                <td data-for="state" width="20">
                    <?
                    switch( $row->state )
                    {
                        case "new":      echo '<span class="fa fa-star  fa-lg"></span>'; break;
                        case "enabled":  echo '<span class="fa fa-check fa-lg"></span>'; break;
                        case "disabled": echo '<span class="fa fa-lock  fa-lg"></span>'; break;
                    }
                    ?>
                </td>
                
                <td nowrap align="center"><?= $row->id_account ?></td>
                
                <td width="60%" colspan="2">
                    
                    <div class="principal_data"><?= convert_emojis($row->display_name) ?></div>
                    
                    <?
                    $principal_data_addons = array();
                    $current_module->load_extensions("records_browser", "principal_data_addons");
                    if( ! empty($principal_data_addons) ){
                        echo "<ul class='principal_addons'><li>";
                        echo implode("</li>\n<li>", $principal_data_addons);
                        echo "</li></ul><hr>";
                    }
                    ?>
                    
                    <div class="actions">
                        <a href="<?= $config->full_root_path ?>/user/<?= $row->user_name ?>" target="_blank">
                            <span class="fa fa-home"></span>
                            <?= $row->user_name ?>
                        </a>
                        
                        <a class="action" target="_blank" href="<?= $config->full_root_path ?>/contact/?target=<?= urlencode($row->user_name) ?>">
                            <span class="fa fa-envelope"></span>
                            <?= $row->email ?>
                        </a>
                        
                        <? if( ! empty($row->alt_email) ): ?>
                            <a class="action" target="_blank" href="<?= $config->full_root_path ?>/contact/?target=<?= urlencode($row->user_name) ?>">
                                <span class="fa fa-envelope"></span>
                                <?= $row->alt_email ?>
                            </a>
                        <? endif; ?>
                        
                        <span class="action pseudo_link" onclick="send_pm(this, '<?= $row->user_name ?>', '<?= replace_escaped_vars($language->contact->pm->send_to, '{$recipient}', htmlspecialchars($row->display_name)) ?>')"
                              title="<?= replace_escaped_vars( $language->contact->pm->title, '{$website_name}', $settings->get("engine.website_name") ) ?>">
                            <span class="fa fa-inbox"></span>
                            <?= $language->contact->pm->caption ?>
                        </span>
                    </div>
                    
                    <div class="actions">
                        
                        <? render_user_devices_trigger($row); ?>
                        
                        <? if($row->id_account != $account->id_account): ?>
                            <a class="action" href="<?= $_SERVER["PHP_SELF"] ?>?mode=edit&id_account=<?= $row->id_account ?>&wasuuup=<?= md5(mt_rand(1, 65535)) ?>">
                                <span class="fa fa-pencil"></span>
                                <?= $current_module->language->admin->record_nav->actions->edit ?>
                            </a>
                        <? endif ?>
                        
                        <? if($row->state == "new"): ?>
                            
                            <? if($row->id_account != $account->id_account): ?>
                                <span class="action pseudo_link" onclick="toggle_account('<?= $row->id_account ?>', 'enable')">
                                    <span class="fa fa-unlock"></span>
                                    <?= $current_module->language->admin->record_nav->actions->enable ?>
                                </span>
                            <? endif; ?>
                            
                        <? elseif($row->state == "enabled"): ?>
                            
                            <? if($row->id_account != $account->id_account): ?>
                                <span class="action pseudo_link" onclick="toggle_account('<?= $row->id_account ?>', 'disable')">
                                    <span class="fa fa-lock"></span>
                                    <?= $current_module->language->admin->record_nav->actions->disable ?>
                                </span>
                            <? endif; ?>
                            
                        <? elseif($row->state == "disabled"): ?>
                            
                            <? if($row->id_account != $account->id_account): ?>
                                <span class="action pseudo_link" onclick="toggle_account('<?= $row->id_account ?>', 'enable')">
                                    <span class="fa fa-unlock"></span>
                                    <?= $current_module->language->admin->record_nav->actions->enable ?>
                                </span>
                            <? endif; ?>
                            
                        <? endif; ?>
                    </div>
                    
                </td>
                
                <td width="20%"><?= $row->country_name ?></td>
                
                <td nowrap align="center">
                    <? if($row->id_account == $account->id_account || $row->level > $account->level): ?>
                        <?
                        if( isset($config->user_levels_by_level[$row->level]) )
                            echo "[{$row->level}] " . $config->user_levels_by_level[$row->level];
                        else
                            echo $row->level;
                        ?>
                    <? else: ?>
                        <div class="user_level_switcher">
                            <div class="current">
                                <?
                                if( isset($config->user_levels_by_level[$row->level]) )
                                    echo "[{$row->level}] " . $config->user_levels_by_level[$row->level];
                                else
                                    echo $row->level;
                                ?>
            
                                <? if( $account->level >= $config::MODERATOR_USER_LEVEL ): ?>
                                    <span class="fa fa-pencil fa-fw pseudo_link"
                                          onclick="open_level_switcher(this, '<?= $row->id_account ?>', '<?= $row->level ?>')"></span>
                                <? endif; ?>
                            </div>
                            <div class="target" style="display: none;"></div>
                        </div>
                    <? endif; ?>
                </td>
                
                <td width="20" align="center">
                    <? $online_flag = $row->last_activity >= date("Y-m-d H:i:s", strtotime("now - 1 minute")) ? "online" : "offline"; ?>
                    <img src="<?= $config->full_root_path ?>/media/icons/user-<?= $online_flag ?>.png" border="0" width="16" height="16"><br>
                    <?= time_elapsed_string_alternate($row->last_activity) ?>
                </td>
                
                <td nowrap align="center">
                    <?= time_elapsed_string($row->creation_date) ?>
                    <span class="fa pseudo_link fa-globe fa-fw" title="<?= $row->creation_host ?>"
                          onclick="alert('<?= $current_module->language->admin->record_nav->row_extras->creation_host ?>\n<?= str_replace("; ", "\\n", $row->creation_host) ?>')"></span>
                </td>
                <td nowrap align="center"><?= time_elapsed_string($row->last_update) ?></td>
                
                <? $current_module->load_extensions("records_browser", "additional_columns"); ?>
                
            </tr>
        <? endwhile; ?>
        </tbody>
    </table>
</div>

<div class="pagination">
    <? $browser->render_pagination_controls("paginate", $pagination); ?>
</div><!-- /.pagination -->

<?
function render_user_devices_trigger($row)
{
    global $database, $current_module;
    
    $res2 = $database->query("
        select
            account_devices.*,
            ( select concat(login_date, '\t', ip, '\t', hostname, '\t', location) 
              from account_logins 
              where account_logins.id_account = account_devices.id_account 
              and account_logins.id_device = account_devices.id_device 
              order by login_date desc limit 1 ) as last_login_info
        from
            account_devices
        where
            id_account = '$row->id_account'
        order by
            last_activity desc
    ");
    
    if( $database->num_rows($res2) == 0 ) return;
    ?>
    
    <span class="action pseudo_link" onclick="show_discardable_dialog('#df_<?= $row->id_account ?>', true)">
        <span class="fa fa-search"></span>
        <?= $current_module->language->admin->record_nav->actions->show_devices ?>
    </span>
    <div id="df_<?= $row->id_account ?>" title="<?= $current_module->language->admin->record_nav->devices_and_login_infos ?>" style="display: none;">
        <? render_user_devices_info($row, $res2) ?>
    </div>
    
    <?
}

function render_user_devices_info($row, $res2)
{
    global $current_module, $database;

    $device_count        = $database->num_rows($res2);
    $changelog_displayed = false;
    ?>
    <table class="nav_table">
        <thead>
        <tr>
            <th><?= $current_module->language->admin->record_nav->devices_columns->id_device ?></th>
            <th><?= $current_module->language->admin->record_nav->devices_columns->label ?></th>
            <th><?= $current_module->language->admin->record_nav->devices_columns->header ?></th>
            <th><?= $current_module->language->admin->record_nav->devices_columns->created ?></th>
            <th><?= $current_module->language->admin->record_nav->devices_columns->state ?></th>
            <th><?= $current_module->language->admin->record_nav->devices_columns->last_activity->header ?></th>
            <th><?= $current_module->language->admin->record_nav->changelog ?></th>
        </tr>
        </thead>
        <tbody>
        <? while( $row2 = $database->fetch_object($res2) ): ?>
            <tr>
                <td><?= $row2->id_device ?></td>
                <td><?= empty($row2->device_label) ? "&mdash;" : $row2->device_label ?></td>
                <td><?= $row2->device_header ?></td>
                <td><?= time_elapsed_string($row2->creation_date) ?></td>
                <td><?= $current_module->language->admin->record_nav->devices_columns->state->{$row2->state} ?></td>
                <td>
                    <?= $current_module->language->admin->record_nav->devices_columns->last_activity->last_action ?> <?= time_elapsed_string($row2->last_activity) ?><br>
                    
                    <? list($last_login, $ip, $hostname, $location) = explode( "\t", $row2->last_login_info ); ?>
                    <?= $current_module->language->admin->record_nav->devices_columns->last_activity->last_login ?> <?= time_elapsed_string($last_login) ?><br>
                    <?= $current_module->language->admin->record_nav->devices_columns->last_activity->ip ?> <?= $ip ?><br>
                    <?= $current_module->language->admin->record_nav->devices_columns->last_activity->hostname ?> <?= $hostname ?><br>
                    <?= $current_module->language->admin->record_nav->devices_columns->last_activity->location ?> <?= $location ?><br>
                </td>
                <? if( ! $changelog_displayed ): ?>
                    <td rowspan="<?= $device_count ?>">
                        <div style="max-height: <?= 80 * $device_count ?>px; overflow: auto;">
                            <ul style="margin: 0; padding-left: 18px;">
                                <?
                                if( empty($row->changelog) )
                                {
                                    echo "&mdash;";
                                }
                                else
                                {
                                    $changelog_lines = split("\n\n", trim($row->changelog));
                                    $changelog_lines = array_reverse($changelog_lines);
                                    foreach( $changelog_lines as $this_line )
                                        echo "<li>" . nl2br($this_line) . "</li>\n";
                                } # end if
                                $changelog_displayed = true;
                                ?>
                            </ul>
                        </div>
                    </td>
                <? endif; ?>
            </tr>
        <? endwhile; ?>
        </tbody>
    </table>
    <?
}
?>
