<?php

/**
 * @file
 * Contains RemoteEntityQuery classes for the NextEuropa Cookie Consent module.
 */

/**
 * Select query for our remote data.
 */
class NextEuropaCookieConsentSelectQuery extends RemoteEntitySelectQuery {

  /**
   * Determines whether the query is RetrieveMultiple or Retrieve.
   *
   * The query is Multiple by default, until an ID condition causes it to be
   * single.
   */
  public $retrieveMultiple = TRUE;

  /**
   * The total record count returned by the query.
   *
   * This is populated after execution if $returnRecordCount was set to TRUE.
   */
  public $totalRecordCount = NULL;

  /**
   * The fields to retrieve.
   */
  public $fields = array();

  /**
   * Relationships.
   */
  public $relationships = array();

  /**
   * Add a condition to the query.
   *
   * Based on EntityFieldQuery::entityCondition().
   *
   * @param $name
   *   The name of the entity property. We only support 'entity_id' for now.
   */
  public function entityCondition($name, $value, $operator = NULL) {
    $this->retrieveMultiple = is_array($value);

    if ($name == 'entity_id') {
      $field = $this->entity_info['remote entity keys']['remote id'];
      $this->conditions[$this->remote_base][] = array(
        'field' => $field,
        'value' => $value,
        'operator' => $operator,
      );
    }
    else {
      $this->conditions[$this->remote_base][] = array(
        'field' => $name,
        'value' => $value,
        'operator' => $operator,
      );
    }
  }

  /**
   * Run the query and return a result.
   *
   * @return array
   *   An array of entity objects as retrieved from the remote connection,
   *   keyed numerically.
   *   An empty array is returned if the response contains no entities.
   *
   * @todo Improve this.
   */
  public function execute() {
    $path = $this->remote_base . '/_design/cookies/_rewrite/cookies';
    if (!empty($this->conditions)) {
      $entity_id = $this->conditions[$this->remote_base][0]['value'];
      // @todo fix this hack
      if (is_array($entity_id)) $entity_id = reset($entity_id);
      $path .= '/' . $entity_id;
    }

    // Make the request.
    $response = $this->connection->callMethodArray('GET', array($path));

    // Parse the response into entities.
    $remote_entities = $this->parseResponse($response);
    return $remote_entities;
  }

  /**
   * Helper method to parse the response.
   *
   * May also set the $totalRecordCount property on the query, if applicable.
   *
   * The results will be post processed by
   * @see RemoteEntityAPIDefaultController::process_remote_entities()
   *
   * @param object $response
   *   The JSON response from REST.
   *
   * @return array
   *   An array of entity objects, keyed numerically. An empty array is returned
   *   if the response contains no entities.
   */
  public function parseResponse($response) {
    $remote_entities = array();

    // Retrieve all rows.
    if (isset($response->rows)) {
      $this->totalRecordCount = (int) $response->total_rows;

      foreach ($response->rows as $row) {
        $remote_entities[] = $row->value;
      }
    }
    elseif (isset($response->_id)) {
      $remote_entities[] = $response;
    }

    return $remote_entities;
  }

}

/**
 * Update query for our remote data.
 */
class NextEuropaCookieConsentUpdateQuery extends RemoteEntityUpdateQuery {

  /**
   * Execute the query.
   */
  public function execute() {
    $path = $this->remote_base . '/' . $this->entity->remote_id;
    $content = drupal_json_encode($this->entity->entity_data);

    // Save document.
    $response = $this->connection->callMethodArray('PUT', array($path, $content));

    // There was an error. Throw an exception!
    if (!isset($response->id)) {
      // Give a random failure id to avoid further saving issues.
      $this->entity->remote_id = uniqid('remote-save-failed-', TRUE);
      throw new Exception(t('Remote save failed: @error (@reason)',
        array('@error' => $response->error, '@reason' => $response->reason)));
    }

    return (isset($response->id)) ? $response->id : FALSE;
  }

}

/**
 * Insert query for our remote data.
 */
class NextEuropaCookieConsentInsertQuery extends NextEuropaCookieConsentUpdateQuery {

  /**
   * Execute the query.
   *
   * The entity must already have been set with setEntity().
   */
  public function execute() {
    // Just ensures the id isn't set then give it to the update handler.
    $this->entity->remote_id = NULL;
    return parent::execute();
  }

}

/**
 * Bulk save query for our remote data.
 */
class NextEuropaCookieConsentBulkSaveQuery extends RemoteEntityBulkSaveQuery {
}
