<?php

/**
 * @file
 * Page callbacks for the NextEuropa Cookie Consent module.
 */

/**
 * Page callback: Displays the cookie consent page.
 *
 * Based on the template available at
 * http://ec.europa.eu/ipg/basics/legal/cookies/index_en.htm
 *
 * @return array
 *   A render array.
 */
function nexteuropa_cookie_consent_page() {
  $cookies = nexteuropa_cookie_consent_sync_cookies();
  if (!empty($cookies)) {
    return theme('nexteuropa_cookie_consent_page', array('notice' => NULL, 'cookies' => $cookies));
  }
  else {
    // @todo variable?
    return t('The Information Providers Guide site does not use any cookies.');
  }
}
