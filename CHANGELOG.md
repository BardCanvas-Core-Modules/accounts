
# Accounts Module Change Log

## [1.20.1] - 2019-02-06

- Added registration validation cases.

## [1.20.0] - 2019-01-29

- Added markup changes to facilitate hooks on the user login and account edition forms.
- Added extension points.

## [1.19.0] - 2018-12-16

- Tuned modules & editable prefs caches.

## [1.18.0] - 2018-11-22

- Added API keys management support.

## [1.17.1] - 2018-09-28

- Added extension points on the registration form and account confirmation page.

## [1.17.0] - 2018-09-07

- Added notification to accounts with missing email set on profile.
- Restructured level check on the toolbox and registrations chart.

## [1.16.1] - 2018-08-04

- Refactored editable prefs generator to properly handle user selected language.

## [1.16.0] - 2018-03-16

- Implemented Google ReCaptcha API v2.
- Added accounts browser page return pointers for addition/edition forms.
- Added records browser support for custom filters (via extensions).
- Added extension points on admin edition/creation forms to allow field injections.

## [1.15.6] - 2018-03-10

- Added missing language captions.
- Fixed empty alert when trying to login using an email address.

## [1.15.5] - 2017-14-12

- Fixed collision on the profile preferences editor caused when loading the editable preferences collection.

## [1.15.4] - 2017-11-18

- Added cache purging after accounts disabling/deletion.

## [1.15.3] - 2017-08-18

- Added search by preferences on the users browser. syntax: `pref:name:=value`

## [1.15.2] - 2017-08-11

- Fixed issue on the online users notification sender.
- Added searching by creation host/ip on the accounts browser.

## [1.15.1] - 2017-07-18

- Added "full layout" option for specs in preferences editor-

## [1.15.0] - 2017-07-14

- Added blocking to login forms on submission.
- Added extension points on login, registration and account editing forms.
- Minor layout adjustments to registration/account editing forms.
- Minor language fixes.

## [1.14.3] - 207-07-03

- Improved preferences editor responsiveness.
- Added extension points to the accounts browser.

## [1.14.2] - 2017-06-22

- Added exension point on account confirmation.

## [1.14.1] - 2017-06-19

- Fixed login redirect for subdirectory based installs.

## [1.14.0] - 2017-06-16

- Added support for email domain blacklists.

## [1.13.2] - 2017-06-08

- Added extension points on the toolbox.

## [1.13.1] - 2017-06-08

- Replaced error message when a user has no open account and tries to set an engine pref with a false confirmation.
- Added extension points on the toolbox and the accounts admin editor.

## [1.13.0] - 2017-05-23

- Added support for "external" preference fields.

## [1.12.2] - 2017-05-17

- Language adjustments.

## [1.12.1] - 2017-05-16

- Removed the broadcast notification helper when the messaging module is not present or disabled.

## [1.12.0] - 2017-05-13

- Renamed file for right sidebar "meta" widget.
- Added missing widget infos in language file.
- Minor tuning of the online users notification helper.

## [1.11.0] - 2017-05-05

- Added helper to broadcast a notification to all online users.

## [1.10.7] - 2017-04-23

- Added fields to user profile info section.

## [1.10.6] - 2017-04-19

- Removed bogus captcha check from registration form.
- Added option to hide birthdate input on create/edit form.

## [1.10.5] - 2017-04-08

- Added message to users trying to login with an email address.
- Some fixes to changelog.

## [1.10.4] - 2017-03-30

- Added missing checks on blacklists for account registration.
- Added changelog.

## [1.10.3] - 2017-03-27

- Added extension points for developers.
- Implemented blacklists for user and display names on registration.
