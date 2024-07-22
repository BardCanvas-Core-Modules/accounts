
<h1><?= $current_module->language->register_form->form_submitted->title ?></h1>

<?= $current_module->language->register_form->form_submitted->content ?>

<?= $settings->get("modules:accounts.confirmation_page_markup") ?>

<? $current_module->load_extensions("register_form", "after_successful_creation"); ?>
