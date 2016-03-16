<?php
/**
 * @file
 * Nexteuropa_multilingual.callbacks.inc.
 */

/**
 * Callback functions for the nexteuropa language negotiation providers.
 */
function nexteuropa_multilingual_language_negotiation_provider_callback($languages) {
  $language_url = FALSE;
  if (!language_negotiation_get_any('nexteuropa_multilingual_url_suffix')) {
    return $language_url;
  }

  // $_GET['q'] might not be available at this time, because
  // path initialization runs after the language bootstrap phase.
  list($language, $_GET['q']) = nexteuropa_multilingual_language_negotiation_split_suffix(isset($_GET['q']) ? $_GET['q'] : NULL, $languages);
  if ($language !== FALSE) {
    $language_url = $language->language;
  }
  return $language_url;
}


/**
 * Rewrites URLs for the "NextEuropa URL suffix" language provider.
 *
 * Adds language suffix to urls. Based on locale_language_url_rewrite_url().
 *
 * @see url().
 * @see locale_url_outbound_alter().
 * @see locale_language_url_rewrite_url().
 */
function nexteuropa_multilingual_language_negotiation_url_rewrite_callback(&$path, &$options) {
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['languages'] = &drupal_static(__FUNCTION__);
  }
  $languages = &$drupal_static_fast['languages'];

  if (!isset($languages)) {
    $languages = language_list('enabled');
    $languages = array_flip(array_keys($languages[1]));
  }

  // Language can be passed as an option, or we go for current URL language.
  if (!isset($options['language'])) {
    global $language_url;
    $options['language'] = $language_url;
  }
  // We allow only enabled languages here.
  elseif (!isset($languages[$options['language']->language])) {
    unset($options['language']);
    return;
  }

  if (!empty($path) && isset($options['language'])) {
    $language = isset($options['language']) && isset($options['language']->language) ? $options['language']->language : '';
    $path = drupal_get_path_alias($path == '<front>' ? variable_get('site_frontpage', 'node') : $path, $language);
    $options['alias'] = TRUE;

    $delimiter = variable_get('nexteuropa_multilingual_language_suffix_delimiter', '_');
    $language_suffix = _nexteuropa_multilingual_get_language_suffix($options['language']);
    $path .= $delimiter . $language_suffix;
  }
}

/**
 * Extract the suffix from the path.
 */
function nexteuropa_multilingual_language_negotiation_split_suffix($path, $languages) {
  $delimiter = variable_get('nexteuropa_multilingual_language_suffix_delimiter', '_');
  $args = empty($path) ? array() : explode($delimiter, $path);
  $suffix = array_pop($args);
  // Handle if there are extra parameters after the suffix.
  $extra_params = explode("/", $suffix);
  $suffix = array_shift($extra_params);
  // Search prefix within enabled languages.
  foreach ($languages as $language) {
    $language_suffix = _nexteuropa_multilingual_get_language_suffix($language);
    if ($language_suffix && $language_suffix == $suffix) {
      // Rebuild $path with the language removed.
      $path = implode($delimiter, $args);
      // Handle the path homepage.
      $path = ($path == 'index') ? '' : $path;
      // Add extra params after suffix.
      if (count($extra_params) > 0) {
        $path .= "/" . implode("/", $extra_params);
      }
      return array($language, $path);
    }
  }
  return array(FALSE, $path);
}

/**
 * Function nexteuropa_multilingual_language_negotiation_switcher_url.
 *
 * @todo: Add documentation.
 */
function nexteuropa_multilingual_language_negotiation_switcher_url($type, $path) {
  $languages = language_list('enabled');
  $links = array();
  foreach ($languages[1] as $language) {
    $links[$language->language] = array(
      'href' => $path,
      'title' => $language->native,
      'language' => $language,
      'attributes' => array('class' => array('language-link')),
    );
  }
  return $links;
}

/**
 * Return the language suffix for the language passed as parameter.
 *
 * Suffix corresponds to language prefix. If not defined, the langcode is used.
 * This suffix is used on the last part of the URL.
 */
function _nexteuropa_multilingual_get_language_suffix($language) {
  if (is_object($language) && isset($language->language)) {
    if (isset($language->prefix) && !empty($language->prefix)) {
      return $language->prefix;
    }
    elseif (!empty($language->language)) {
      return $language->language;
    }
  }
  return NULL;
}
