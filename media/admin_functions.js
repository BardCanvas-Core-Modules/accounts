
function toggle_registration_mode(new_mode)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        mode: new_mode == 'on' ? 'enable_register' : 'disable_register',
        wasuuup: wasuuup()
    };
    
    $.blockUI(blockUI_default_params);
    stop_notifications_getter();
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $.unblockUI();
            start_notifications_getter();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + wasuuup();
    });
}

function switch_admin(id_account, admin_action)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       admin_action,
        wasuuup:    wasuuup()
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    stop_notifications_getter();
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            start_notifications_getter();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + wasuuup();
    });
}

function toggle_account(id_account, new_mode, trigger, callback)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       new_mode,
        wasuuup:    wasuuup()
    };
    
    var $tr;
    if( trigger ) $tr = $(trigger);
    else $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    
    stop_notifications_getter();
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            start_notifications_getter();
            
            return;
        }
        
        if( callback )
        {
            $tr.unblock();
            start_notifications_getter();
            callback();
        }
    });
}

function delete_account(id_account)
{
    var message = $('#deletion_confirmation_message').text();
    if( ! confirm(message) ) return;
    
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       'delete',
        wasuuup:    wasuuup()
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $.blockUI(blockUI_default_params);
    
    // stop_notifications_getter();
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $.unblockUI();
            // start_notifications_getter();
            
            return;
        }
    
        $.unblockUI();
        $tr.fadeOut('fast', function() { $tr.remove(); });
    });
}

function open_level_switcher(trigger, id_account, current_level, reload_on_change)
{
    if( typeof reload_on_change == 'undefined' ) reload_on_change = true;
    
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
        wasuuup:    wasuuup()
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    if( $tr.length == 0 ) $tr = $trigger.closest('.user_level_switcher');
    $tr.block(blockUI_smallest_params);
    stop_notifications_getter();
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            start_notifications_getter();
            
            return;
        }
        
        if( reload_page != 'true' )
        {
            $tr.unblock();
            start_notifications_getter();
            
            return;
        }
        
        if( $('body').hasClass('admin') && document.filter_form )
            $('#filter_form').submit();
        else
            location.href = $_PHP_SELF + '?wasuuup=' + wasuuup();
    });
}

function prepare_online_users_notification_submission(formData, $form, options)
{
    $form.closest('.ui-dialog').block(blockUI_medium_params);
}

function process_online_users_notification_response(responseText, statusText, xhr, $form)
{
    $form.closest('.ui-dialog').unblock();
    
    if( responseText != 'OK' )
    {
        alert( responseText );
        
        return;
    }
    
    $('#online_users_notification_dialog').dialog('close');
}

function notify_online_users()
{
    $('#online_users_notification')[0].reset();
    $('#online_users_notification_dialog').dialog('open');
}

$(document).ready(function()
{
    $('#online_users_notification').ajaxForm({
        target:       '#online_users_notification_target',
        beforeSubmit: prepare_online_users_notification_submission,
        success:      process_online_users_notification_response
    });
    
    var $dialog        = $('#online_users_notification_dialog');
    var submit_caption = $dialog.attr('data-ok-caption');
    var cancel_caption = $dialog.attr('data-cancel-caption');
    
    $dialog.dialog({
        modal:    true,
        autoOpen: false,
        buttons:  [
            {
                text:  cancel_caption,
                icons: { primary: "ui-icon-cancel" },
                click: function() { $(this).dialog( "close" ); }
            },
            {
                text: submit_caption,
                icons: { primary: "ui-icon-check" },
                click: function() { $('#online_users_notification').submit(); }
            }
        ]
    });
});
