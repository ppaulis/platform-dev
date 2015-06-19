<?php

/**
 * @file
 * Contains Clients connection classes.
 */

/**
 * Generic CouchDB Clients connection class.
 * Largely based on the CouchDB Integration module.
 * See http://cgit.drupalcode.org/couchdb/tree/?h=7.x-1.x
 */
class clients_connection_couchdb_rest extends clients_connection_base
  implements ClientsConnectionAdminUIInterface {

  /**
   * Additional cURL options to use.
   */
  public $additionalCurlOptions = array();

  /**
   * The cURL handle.
   */
  protected $curlHandle = NULL;

  /**
   * Initializes the cURL connection.
   */
  protected function curlInitialize() {
    if (!isset($this->curlHandle)) {
      $this->curlHandle = curl_init();
      $curl_options = $this->additionalCurlOptions + array(
        CURLOPT_URL => $this->endpoint,
        CURLOPT_FOLLOWLOCATION => TRUE,
        CURLOPT_MAXREDIRS => 5,
        CURLOPT_RETURNTRANSFER => TRUE,
        CURLOPT_SSL_VERIFYPEER => FALSE,
        CURLOPT_SSL_VERIFYHOST => FALSE,
        CURLOPT_HEADERFUNCTION => array(&$this, 'curlHeaderCallback'),
      );

      // Basic authentication.
      $this->credentialsLoad();
      if (!empty($this->credentials)) {
        $username = $this->credentials['username'];
        $password = $this->credentials['password'];
        $curl_options[CURLOPT_USERPWD] = $username . ':' . $password;
      }
      curl_setopt_array($this->curlHandle, $this->additionalCurlOptions + $curl_options);
    }
  }

  /**
   * Execute a cURL request.
   *
   * @param array $options
   *   An array of options to pass to cURL using curl_setopt_array().
   *
   * @return mixed
   *   Whatever is returned from json_decode after curl_exec.
   */
  protected function curlExec($options) {
    $this->curlInitialize();
    curl_setopt_array($this->curlHandle, $this->additionalCurlOptions + $options);

    $this->headers = array();
    $this->content = curl_exec($this->curlHandle);
    $this->url = curl_getinfo($this->curlHandle, CURLINFO_EFFECTIVE_URL);
    $this->status = curl_getinfo($this->curlHandle, CURLINFO_HTTP_CODE);

    // Automatically decode JSON-encoded responses.
    if ($content = json_decode($this->content)) {
      $this->content = $content;
    }

    // Close the curl handler - Reset the handle for the next request.
    curl_close($this->curlHandle);
    $this->curlHandle = NULL;

    return $this->content;
  }

  /**
   * Reads headers.
   *
   * @param resource $curl
   *   The cURL handler.
   * @param string $header
   *   A header.
   *
   * @return int
   *   The string length of $header.
   */
  protected function curlHeaderCallback($curl, $header) {
    if (preg_match('/^([^:]+): (.*)$/', $header, $matches)) {
      list(, $name, $value) = $matches;
      $this->headers[$name][] = $value;
    }

    // This is required by cURL.
    return strlen($header);
  }

  /**
   * Get the content retrieved by the last request.
   */
  public function getContent() {
    return $this->content;
  }

  /**
   * Get the headers of the response to the last request.
   */
  public function getHeaders() {
    return $this->headers;
  }

  /**
   * Get the headers of the response to the last request.
   */
  public function getStatus() {
    return $this->status;
  }

  /**
   * Execute a GET request to the CouchDB server.
   *
   * @param string $path
   *   The path to use, relative to the CouchDB URL.
   * @param array $headers
   *   An array containing additional HTTP request headers, each formatted as
   *   "name: value".
   *
   * @return mixed
   *   The response, also available as $this->getContent().
   *   Headers are available as $this->getHeaders().
   */
  public function getMethod($path, array $headers = array()) {
    return $this->curlExec(array(
      CURLOPT_URL => $this->endpoint . '/' . $path,
      CURLOPT_CUSTOMREQUEST => 'GET',
      CURLOPT_HTTPGET => TRUE,
      CURLOPT_NOBODY => FALSE,
      CURLOPT_HTTPHEADER => $headers,
    ));
  }

  /**
   * Execute a POST request to the CouchDB server.
   *
   * @param string $path
   *   The path to use, relative to the CouchDB URL.
   * @param string $values
   *   The values to send to the server.
   * @param array $headers
   *   An array containing additional HTTP request headers, each formatted as
   *   "name: value".
   *
   * @return mixed
   *   The response, also available as $this->getContent().
   *   Headers are available as $this->getHeaders().
   */
  public function postMethod($path, $values = '', array $headers = array()) {
    return $this->curlExec(array(
      CURLOPT_URL => $this->endpoint . '/' . $path,
      CURLOPT_CUSTOMREQUEST => 'POST',
      CURLOPT_POST => TRUE,
      CURLOPT_POSTFIELDS => $values,
      CURLOPT_HTTPHEADER => $headers,
    ));
  }

  /**
   * Execute a COPY request to the CouchDB server.
   *
   * This is a non-standard HTTP method, only supported by CouchDB.
   *
   * @param string $path
   *   The path to use, relative to the CouchDB URL.
   * @param array $headers
   *   An array containing additional HTTP request headers, each formatted as
   *   "name: value".
   *
   * @return mixed
   *   The response, also available as $this->getContent().
   *   Headers are available as $this->getHeaders().
   */
  public function copyMethod($path, array $headers = array()) {
    return $this->curlExec(array(
      CURLOPT_URL => $this->endpoint . '/' . $path,
      CURLOPT_CUSTOMREQUEST => 'COPY',
      CURLOPT_HTTPHEADER => $headers,
    ));
  }

  /**
   * Execute a PUT request to the CouchDB server.
   *
   * @param string $path
   *   The path to use, relative to the CouchDB URL.
   * @param string $content
   *   The content to send to the server.
   * @param array $headers
   *   An array containing additional HTTP request headers, each formatted as
   *   "name: value".
   *
   * @return mixed
   *   The response, also available as $this->getContent().
   *   Headers are available as $this->getHeaders().
   */
  public function putMethod($path, $content, $headers = array()) {
    // Prepare the content.
    $fp = fopen('php://temp', 'r+');
    fwrite($fp, $content);
    rewind($fp);

    $result = $this->curlExec(array(
      CURLOPT_URL => $this->endpoint . '/' . $path,
      CURLOPT_CUSTOMREQUEST => 'PUT',
      CURLOPT_PUT => TRUE,
      CURLOPT_INFILE => $fp,
      CURLOPT_INFILESIZE => strlen($content),
      CURLOPT_HTTPHEADER => $headers,
    ));

    fclose($fp);
    return $result;
  }

  /**
   * Execute a DELETE request to the CouchDB server.
   *
   * @param string $path
   *   The path to use, relative to the CouchDB URL.
   *
   * @return mixed
   *   The response, also available as $this->getContent().
   *   Headers are available as $this->getHeaders().
   */
  public function deleteMethod($path) {
    return $this->curlExec(array(
      CURLOPT_URL => $this->endpoint . '/' . $path,
      CURLOPT_CUSTOMREQUEST => 'DELETE',
      CURLOPT_HTTPHEADER => array(),
    ));
  }

  /**
   * List databases available in the CouchDB server.
   *
   * @return mixed
   *   Return all the databases in this couchDB.
   */
  public function listDatabases() {
    return $this->getMethod('_all_dbs');
  }

  /**
   * List active tasks in a couchDB.
   *
   * @return mixed
   *   Return all active tasks that are running
   */
  public function getActiveTasks() {
    return $this->getMethod('_active_tasks');
  }

  /**
   * Replicate a database.
   *
   * @param string $source
   *   The source database.
   * @param string $target
   *   The target database.
   * @param bool $continuous
   *   TRUE for continuous replication. Defaults to FALSE.
   * @param bool $cancel
   *   TRUE to cancel replication.
   *
   * @return mixed
   *   Return replication status.
   */
  public function replicateDatabase($source, $target, $continuous = FALSE, $cancel = FALSE) {
    $values = array(
      'source' => $source,
      'target' => $target,
    );
    // Identifies if continuous replication is needed.
    if ($continuous) {
      $values['continuous'] = TRUE;
    }
    // Cancel sends a cancel replication command to couchDB.
    if ($cancel) {
      $values['cancel'] = TRUE;
    }
    return $this->postMethod('_replicate', json_encode($values), array('Content-type: application/json'));
  }

  /**
   * Get UUIDs from CouchDB.
   *
   * @param int $count
   *   The number of ids to return. Defaults to 1 UUID.
   *
   * @return mixed
   *   Returns the requested UUIDs.
   */
  public function getUUIDS($count = 1) {
    return $this->getMethod('_uuids' . (($count > 1) ? '?count=' . $count : ''));
  }

  /**
   * Get server stats.
   *
   * @return mixed
   *   Return couchDB stats.
   */
  public function getStats() {
    return $this->getMethod('_stats');
  }

  /**
   * Create a database.
   *
   * @param string $name
   *   The name of the database to create.
   *
   * @return mixed
   *   Create database status.
   */
  public function createDatabase($name) {
    return $this->putMethod(urlencode($name), '');
  }

  /**
   * Delete a database.
   *
   * @param string $name
   *   The name of the database to delete.
   *
   * @return mixed
   *   Delete database status.
   */
  public function deleteDatabase($name) {
    return $this->deleteMethod(urlencode($name));
  }

  /**
   * Fetch information about a database.
   *
   * @param string $name
   *   The name of the database to retrieve.
   *
   * @return mixed
   *   Database info.
   */
  public function databaseInfo($name) {
    if (!empty($name)) {
      return $this->getMethod(urlencode($name));
    }
    else {
      return NULL;
    }
  }

  // ================================= clients_connection_base overrides.

  /**
   * Call a remote method with an array of parameters.
   *
   * This is intended for internal use from callMethod() and
   * clients_connection_call().
   * If you need to call a method on given connection object, use callMethod
   * which has a nicer form.
   *
   * Subclasses do not necessarily have to override this method if their
   * connection type does not make sense with this.
   *
   * @param string $method
   *   The name of the remote method to call.
   * @param array $method_params
   *   An array of parameters to passed to the remote method.
   *
   * @return mixed
   *   Whatever is returned from the remote site.
   *
   * @throws Exception on error from the remote site.
   *  It's up to subclasses to implement this, as the test for an error and
   *  the way to get information about it varies according to service type.
   *
   * @todo Add validation, throw exceptions, use $this->debug?
   */
  public function callMethodArray($method, $method_params = array()) {
    $response = NULL;
    $path = $method_params[0];

    switch (strtoupper($method)) {
      case 'GET':
        $headers = isset($method_params[1]) ? $method_params[1] : array();
        $response = $this->getMethod($path, $headers);
        break;

      case 'POST':
        $values = isset($method_params[1]) ? $method_params[1] : '';
        $headers = isset($method_params[2]) ? $method_params[2] : array();
        $response = $this->postMethod($path, $values, $headers);
        break;

      case 'PUT':
        $content = isset($method_params[1]) ? $method_params[1] : array();
        $headers = isset($method_params[2]) ? $method_params[2] : array();
        $response = $this->putMethod($path, $content, $headers);
        break;

      case 'DELETE':
        $response = $this->deleteMethod($path);
        break;

      case 'COPY':
        $headers = isset($method_params[1]) ? $method_params[1] : array();
        $response = $this->copyMethod($path, $headers);
        break;
    }

    // Handle any errors and then return the response.
    $this->handleRestError($path, $response);

    return $response;
  }

  // ================================= Local methods.

  /**
   * Common helper for reacting to an error from a REST call.
   *
   * Gets the error from the response, logs the error message,
   * and throws an exception, which should be caught by the module making use
   * of the Clients connection API.
   *
   * @param string $request
   *   The request information.
   * @param object $response
   *   The REST response data, decoded.
   *
   * @throws Exception
   */
  protected function handleRestError($request, $response) {
    // @todo test status response, improve debug.
    // Report and throw an error if we get anything unexpected.
    if (!in_array($this->status, array(200, 201, 202, 204, 404))) {
      // Report error to the logs.
      watchdog('clients', 'Error with REST request (@req). Error was code @code with response "@response".', array(
        '@req' => $request,
        '@code' => $this->status,
        '@response' => print_r($response),
      ), WATCHDOG_ERROR);

      // Throw an error with which callers must deal.
      throw new Exception(t('Clients connection error, got code @code with response "@response".', array(
        '@code' => $this->status,
        '@response' => print_r($response),
      )));
    }
  }

  /**
   * Test the connection.
   *
   * @return bool
   *   State of the login.
   */
  public function testConnection() {
    $response = $this->callMethod('GET', '');
    if (is_object($response) && isset($response->couchdb) && $response->couchdb === 'Welcome') {
      return TRUE;
    }

    return FALSE;
  }

}

/**
 * Set up a client connection to our REST services.
 */
class clients_connection_nexteuropa_cookie_consent_rest
  extends clients_connection_couchdb_rest
  implements ClientsRemoteEntityInterface {

  // ============================= ClientsRemoteEntityInterface implementations.

  /**
   * {@inheritdoc}
   */
  public function remote_entity_load($entity_type, $id) {
    $query = $this->getRemoteEntityQuery('select');
    $query->base($entity_type);
    $query->entityCondition('entity_id', $id);
    $result = $query->execute();

    // There's only one. Same pattern as entity_load_single().
    return reset($result);
  }

  /**
   * {@inheritdoc}
   */
  public function remote_entity_save($entity_type, $entity, $remote_properties = array()) {
    // Determine whether this is an insert or an update. An entity not yet
    // saved remotely will have no remote ID property.
    $action = empty($entity->remote_id) ? 'insert' : 'update';

    $query = $this->getRemoteEntityQuery($action);
    $query->setEntity($entity_type, $entity);

    if ($action == 'insert') {
      return $query->execute();
    }
    else {
      $query->addFields($remote_properties);
      return $query->execute();
    }
  }

  /**
   * {@inheritdoc}
   */
  public function entity_property_type_map() {
    return array(
      'EntityCollection' => 'list<string>',
    );
  }

  /**
   * Get a new RemoteEntityQuery object appropriate for the connection.
   *
   * @param string $query_type
   *   (optional) The type of the query. Defaults to 'select'.
   *
   * @return RemoteEntityQuery
   *   A remote query object of the type appropriate to the query type.
   */
  public function getRemoteEntityQuery($query_type = 'select') {
    switch ($query_type) {
      case 'select':
        return new NextEuropaCookieConsentSelectQuery($this);

      case 'insert':
        return new NextEuropaCookieConsentInsertQuery($this);

      case 'update':
        return new NextEuropaCookieConsentUpdateQuery($this);

      case 'bulk_save':
        return new NextEuropaCookieConsentBulkSaveQuery($this);
    }

    return NULL;
  }

}
