<?php
/**
 * @file   sql_compressed_dump.drush.inc
 * @author Xabier Maeztu <xabier@krask.com>
 * @date   Sun Dec 19 16:34:45 2010
 * 
 * @brief  Implements various drush commands for compressed database dumps.
 * 
 */

//We define the constants for the dump directory and the compression programs
define('DEFAULT_DB_DUMP_DIRNAME', '.drush-dumps');
define('DEFAULT_BZIP2_PROGRAM', '/usr/bin/bzip2');
define('DEFAULT_P7ZIP_PROGRAM', '/usr/bin/p7zip');

/**
* Implementation of hook_drush_help().
*/
function sql_compressed_dump_drush_help($section) {
  switch ($section) {
    case 'drush:sql-gz-dump':
      return dt('Dumps a database and compresses it using gzip');
      break;
    case 'drush:sql-bz-dump':
      return dt('Dumps a database and compresses it using bzip2');
      break;
    case'drush:sql-7z-dump':
      return dt('Dumps a database and compresses it using p7zip');
      break;
  }
}

/**
* Implementation of hook_drush_command().
*/
function sql_compressed_dump_drush_command() {
  $options = array(
    '--database'  => 'The DB connection key if using multiple connections in settings.php.',
    '--result-file' => 'Save to a file. The file should be relative to Drupal root. If you do not define this value, it will be automatically generated.',
    '--skip-tables-key' => 'A key in the $skip_tables array. @see example.drushrc.php. Optional.',
    '--structure-tables-key' => 'A key in the $structure_tables array. @see example.drushrc.php. Optional.',
    '--compression-level=' => 'The bzip algorithm compression level. The default is 6.',
  );
  if (drush_drupal_major_version() >= 7) {
    $options['--target'] = 'The name of a target within the specified database.';
  }
  $items['sql-gz-dump'] = array(
    'callback' => 'drush_sql_gz_dump_execute',
    'description' => 'Exports the Drupal DB as SQL using mysqldump and compresses the dump using gzip. Default location is ~.drush-dumps.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
    'examples' => array(
      'drush sql-gz-dump --result-file=../18.sql' => 'Save SQL dump to the directory above Drupal root.',
      'drush sql-gz-dump --skip-tables-key=common' => 'Skip standard tables. @see example.drushrc.com',
    ),
    'options' => $options,
    'aliases' => array('sqgz'),
  );
  $items['sql-bz-dump'] = array(
    'callback' => 'drush_sql_bz_dump_execute',
    'description' => 'Exports the Drupal DB as SQL using mysqldump and compresses the dump using bzip2. Default location is ~.drush-dumps.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
    'examples' => array(
      'drush sql-bz-dump --result-file=../18.sql' => 'Save SQL dump to the directory above Drupal root.',
      'drush sql-bz-dump --skip-tables-key=common' => 'Skip standard tables. @see example.drushrc.com',
    ),
    'options' => $options,
    'aliases' => array('sqbz'),
  );
  $items['sql-7z-dump'] = array(
    'callback' => 'drush_sql_7z_dump_execute',
    'description' => 'Exports the Drupal DB as SQL using mysqldump and compresses the dump using bzip2. Default location is ~.drush-dumps.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
    'examples' => array(
      'drush sql-7z-dump --result-file=../18.sql' => 'Save SQL dump to the directory above Drupal root.',
      'drush sql-7z-dump --skip-tables-key=common' => 'Skip standard tables. @see example.drushrc.com',
    ),
    'options' => $options,
    'aliases' => array('sq7z'),
  );
  return $items;
} // sql_compressed_dump_drush_command


/**
* Command callback. Outputs the entire Drupal database in SQL format using
* mysqldump.
*/
function drush_sql_bz_dump_execute() {
  static $dumped;
  // Hack to get it to not dump the DB twice.
  // Needs work!
  if (isset($dumped)) {
    return;
  }
  $exec = drush_sql_build_bz_dump_command();
  // Avoid the php memory of the $output array in drush_shell_exec().
  $return = drush_op('system', $exec);
  $dumped = TRUE;
  return $return;
}
