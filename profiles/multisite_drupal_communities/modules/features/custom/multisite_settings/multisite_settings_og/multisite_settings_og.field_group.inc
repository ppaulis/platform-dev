<?php
/**
 * @file
 * multisite_settings_og.field_group.inc
 */

/**
 * Implements hook_field_group_info().
 */
function multisite_settings_og_field_group_info() {
  $export = array();

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_content|node|article|form';
  $field_group->group_name = 'group_article_content';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Content',
    'weight' => '0',
    'children' => array(
      0 => 'body',
      1 => 'field_image',
      2 => 'group_content_access',
      3 => 'og_group_ref',
      4 => 'title',
    ),
    'format_type' => 'tab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-content field-group-tab',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_article_content|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_date|node|article|form';
  $field_group->group_name = 'group_article_date';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Date',
    'weight' => '1',
    'children' => array(
      0 => 'field_article_publication_date',
      1 => 'field_article_unpublish_on',
    ),
    'format_type' => 'tab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-date field-group-tab',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_article_date|node|article|form'] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_article_metadata|node|article|form';
  $field_group->group_name = 'group_article_metadata';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'article';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Metadata',
    'weight' => '2',
    'children' => array(
      0 => 'field_tags',
    ),
    'format_type' => 'tab',
    'format_settings' => array(
      'formatter' => 'closed',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-article-metadata field-group-tab',
        'required_fields' => 1,
      ),
    ),
  );
  $export['group_article_metadata|node|article|form'] = $field_group;

  return $export;
}