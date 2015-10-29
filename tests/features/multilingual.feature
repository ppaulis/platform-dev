@api
Feature: Multilingual features
  In order to easily understand the content of the European Commission
  As a citizen of the European Union
  I want to be able to read content in my native language

  Background:
    Given the following languages are available:
      | languages |
      | en        |
      | fr        |
      | de        |

  Scenario: Content can be translated in available languages
    Given I am viewing a multilingual "page" content:
      | language | title                        |
      | en       | This title is in English     |
      | fr       | Ce titre est en Français     |
      | de       | Dieser Titel ist auf Deutsch |
    Then I should see the heading "This title is in English"
    And I should see the link "Français"
    And I should see the link "Deutsch"
    # Language switcher is broken. Will be fixed in NEXTEUROPA-5895.
    # When I click "Français"
    # And I should see the heading "Ce titre est en Français"
    # When I click "Deutsch"
    # And I should see the heading "Dieser Titel ist auf Deutsch"

  Scenario: Custom URL suffix language negotiation is applied by default on new content.
    Given I am logged in as a user with the 'administrator' role
    And I am viewing a multilingual "page" content:
      | language | title            |
      | en       | Title in English |
      | fr       | Title in French  |
      | de       | Title in German  |
    # This is currently broken. It is not possible to switch to a different
    # language URL. Instead an URL query argument "2nd-language" is appended to
    # the URL. This will be fixed in NEXTEUROPA-5881.
    # Then I should be on "content/title-english_en"
    # When I click "Français"
    # Then I should be on "content/title-english_fr"
    # When I click "Deutsch"
    # Then I should be on "content/title-english_de"

  Scenario: Enable multiple languages
    Given I am logged in as a user with the 'administrator' role
    When I go to "admin/config/regional/language"
    Then I should see "English"
    And I should see "French"
    And I should see "German"

  Scenario: Enable language suffix and check the base path
    Given I am logged in as a user with the 'administrator' role
    When I go to "admin/config/regional/language/configure"
    And I check the box "edit-language-enabled-nexteuropa-multilingual-url-suffix"
    And I uncheck the box "edit-language-enabled-locale-url"
    And I press the "Save settings" button
    Then I should see the success message "Language negotiation configuration saved."
    When I go to "admin/config/regional/language/edit/en"
    And I fill in "edit-prefix" with "en-prefix"
    And I press the "Save language" button
    And I go to "admin/config/system/site-information"
    And I fill in "edit-site-frontpage" with "admin/fake-url"
    And I select "01000" from "edit-classification"
    And I press the "Save configuration" button
    Then I should see the success message "The configuration options have been saved."
    And the cache has been cleared
    And I should not see "admin/fake-url" in the ".form-item-site-frontpage span.field-prefix" element
    And I should not see "en-prefix" in the ".form-item-site-frontpage span.field-prefix" element