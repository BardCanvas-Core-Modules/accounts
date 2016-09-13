
function toggle_registration_mode(new_mode)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        mode: new_mode == 'on' ? 'enable_register' : 'disable_register',
        wasuuup: parseInt(Math.random() * 1000000000000000)
    };
    
    $.blockUI(blockUI_default_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $.unblockUI();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}

function switch_admin(id_account, admin_action)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       admin_action,
        wasuuup:    parseInt(Math.random() * 1000000000000000)
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            
            return;
        }
    
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}

function toggle_account(id_account, new_mode, trigger, reload_page)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       new_mode,
        wasuuup:    parseInt(Math.random() * 1000000000000000)
    };
    
    var $tr;
    if( trigger ) $tr = $(trigger);
    else $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            
            return;
        }
        
        if( ! reload_page )
        {
            $tr.unblock();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}

function open_level_switcher(trigger, id_account, current_level, reload_on_change)
{
    if( typeof reload_on_change == 'undefined' ) reload_on_change = false;
    
    var $target = $(trigger).closest('.user_level_switcher').find('.target');
    var $source = $(trigger).closest('.current');
    var $select = $('#level_switch').find('select').clone();
    // var span    = '<span class="fa fa-ban fa-fw pseudo_link" onclick="cancel_level_switching(this)"></span>';
    
    $select.attr('data-reload-page', (reload_on_change ? 'true' : 'false'));
    $select.attr('data-id-account', id_account);
    $select.find('option[value="' + current_level + '"]').prop('selected', true);
    $source.hide();
    $target.append($select).show();
}

function cancel_level_switching(trigger)
{
    var $target  = $(trigger).closest('.target');
    var $current = $(trigger).closest('.user_level_switcher').find('.current');
    
    $target.html('').hide();
    $current.show();
}

function change_user_level(trigger)
{
    var $trigger    = $(trigger);
    var value       = $trigger.find('option:selected').val();
    var id_account  = $trigger.attr('data-id-account');
    var reload_page = $trigger.attr('data-reload-page');
    
    if( value == '!@cancel' )
    {
        cancel_level_switching(trigger);
        
        return;
    }
    
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        mode:       'change_level',
        id_account: id_account,
        level:      value,
        wasuuup:    parseInt(Math.random() * 1000000000000000)
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    if( $tr.length == 0 ) $tr = $trigger.closest('.user_level_switcher');
    $tr.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            
            return;
        }
        
        if( reload_page != 'true' )
        {
            $tr.unblock();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}
