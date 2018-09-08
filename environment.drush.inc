<?php
/**
 * @file   environment.drush.inc
 *
 */

/**
* Implementation of hook_drush_help().
*/
function environment_drush_help($section) {
  switch ($section) {
    case 'drush:environment':
      return dt('Set environment to development mode or production mode');
  }
}

/**
* Implementation of hook_drush_command().
*/
function environment_drush_command() {
  $items['environment'] = array(
    'callback' => 'drush_environment_execute',
    'description' => 'Set environment to development mode or production mode',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'examples' => array(
      'drush environment dev' => 'Set environment to development mode',
      'drush environment pro' => 'Set environment to development mode',
    ),
    'arguments' => array(
       'env' => 'Environment',
    ),
    'aliases' => array('env'),
  );
  return $items;
}

/**
* Command callback. environment
*/
function drush_environment_execute($env) {
  $callback = 'drush_environment_execute_' . $env;
  if (function_exists($callback)) {
    return $callback();
  }
  drush_print("Please check examples to know how to use this command");
  return FALSE;
}

function drush_environment_execute_dev() {
  // Enable dev friendly modules.
  module_enable(array('dblog', 'update', 'field_ui'), TRUE);
  module_enable(array('devel'), TRUE);
  module_enable(array('views_ui'), TRUE);
  module_enable(array('context_ui'), TRUE);
  module_enable(array('stage_file_proxy'), TRUE);
  module_enable(array('reroute_email'), TRUE);
  drush_log(dt('Enabled dev modules'), 'ok');
  // Disable any production modules that you don't want to run locally, like CDN.
  $disable = array('boost','securepages','cdn', 'advagg', 'memcache');
  module_disable($disable);
  drush_log(dt('Disabled live modules'), 'ok');
  // Make sure the rerouting of email is turned on so we don't send emails to actual users from our local installations.
  if(module_exists('reroute_email')) {
    variable_set('reroute_email_enable', 1);
    variable_set('reroute_email_address', 'oliver@jufist.com');
    drush_log("Email is being rerouted to oliver@jufist.com.", 'ok');
  } else {
    drush_log('Emails will be sent to users!', 'warning');
  }
  // Allow everyone to see devel messages like dpm().
  if(module_exists('devel')) {
    user_role_grant_permissions(1, array('access devel information'));
    user_role_grant_permissions(2, array('access devel information'));
  }
  // Set some dev-friendly settings
  variable_set('cache', "0");
  variable_set('block_cache', "0");
  variable_set('error_level', "2");
  variable_set('preprocess_js', "0");
  variable_set('preprocess_css', "0");
  variable_set('page_compression', "0");
  drush_log('Page cache, page compression, JS optimization, and CSS optimization disabled.', 'ok');
  drupal_flush_all_caches();
  drush_log('All caches cleared.', 'ok');
  drush_log('Site ready for development!', 'ok');
}

function drush_environment_execute_prod() {
  // Enable dev friendly modules.
  module_disable(array('devel', 'views_ui', 'context_ui', 'stage_file_proxy', 'reroute_email'), TRUE);
  drush_log(dt('Disabled dev modules'), 'ok');
  module_enable(array('boost','securepages','cdn', 'advagg', 'memcache'));
  drush_log(dt('Enabled live modules'), 'ok');
  // Make sure the rerouting of email is turned on so we don't send emails to actual users from our local installations.
  if(module_exists('reroute_email')) {
    variable_set('reroute_email_enable', 0);
    drush_log("Email is back to normal", 'ok');
  }
  // Set some dev-friendly settings
  variable_set('cache', 1);
  variable_set('block_cache', 1);
  variable_set('error_level', 0);
  variable_set('preprocess_js', 1);
  variable_set('preprocess_css', 1);
  variable_set('page_compression', 1);
  drush_log('Page cache, page compression, JS optimization, and CSS optimization enabled.', 'ok');
  drupal_flush_all_caches();
  drush_log('All caches cleared.', 'ok');
  drush_log('Site ready for production!', 'ok');
}
