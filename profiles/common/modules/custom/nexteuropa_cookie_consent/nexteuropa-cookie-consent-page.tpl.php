<?php

/**
 * @file
 * Default theme implementation to display the cookie consent page.
 *
 * Available variables:
 * @todo
 * -
 * -
 *
 * @see template_preprocess()
 * @see template_preprocess_nexteuropa_cookie_consent_page() @todo
 * @see template_process()
 *
 * @ingroup themeable
 */

// @todo change this dirty stuff and use wrappers.
$cookies_render = entity_view('nexteuropa_cookie_consent', $cookies);
?>
<p>Cookie Consent page output</p>

<?php print drupal_render($cookies_render); ?>
