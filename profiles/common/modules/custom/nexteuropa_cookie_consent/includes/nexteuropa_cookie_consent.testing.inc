<?php

/**
 * @file
 * Contains Clients testing classes for the NextEuropa Cookie Consent module.
 */

/**
 * Test basic connection to the remote server.
 */
class ClientsConnectionNextEuropaCookieConsentTestConnection
  implements ClientsConnectionTestingInterface {

  /**
   * {@inheritdoc}
   */
  public function testLabels() {
    return array(
      'label' => t('Test Connection'),
      'description' => t('Test the connection with the configured credentials.'),
      'button' => t('Connect'),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function test($connection, &$button_form_values) {
    $logged_in = FALSE;

    try {
      $logged_in = $connection->testConnection();
    }
    catch (Exception $e) {
      drupal_set_message(t('Could not connect to the remote site, got error message "@message".', array(
        '@message' => $e->getMessage(),
      )), 'warning');
    }

    if ($logged_in) {
      drupal_set_message(t('Sucessfully connected to the CCK service.'));
    }
    else {
      drupal_set_message(t('Could not connect to the CCK service.'), 'warning');
    }

    return $connection->getContent();
  }
}
