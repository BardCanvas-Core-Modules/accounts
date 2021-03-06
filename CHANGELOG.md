
# Accounts Module Change Log

## [1.22.2] - 2021-07-20

- Fixed wrong keys used when encrypting API keys before saving an account edited by an admin. 

## [1.22.1] - 2021-07-05

- Added toogle for showing/hiding the password on the login dialog.

## [1.22.0] - 2021-02-11

- Added workaround for a potential invalid type casting on api key listings in the profile editor.
- Added support for 2FA.
- Tuned user display name in the header menu.

## [1.21.5] - 2019-09-06

- Added registration:after_sending_confirmation_email extension point.

## [1.21.4] - 2019-09-06

- Tuned checks for automatic generation of user names.
- Added section identifiers to login forms.
- Added login script extension point.

## [1.21.3] - 2019-08-03

- Added extension points.

## [1.21.2] - 2019-07-24

- Added setting of default country when it can't be obtained on registration.

## [1.21.1] - 2019-07-22

- Hidden country selector when set as non-mandatory on the account editor form.
- Tuned profile saving scripts.
- Added extension points.

## [1.21.0] - 2019-06-07

- Devices code cleanup.
- Added reload button to devices page.
- Removed device from screen instead of reloading the whole page when deleting a device.

## [1.20.6] - 2019-05-21

- Added password recovery check to avoid leakages.

## [1.20.5] - 2019-03-19

- Added extension point on the user dropdown menu.

## [1.20.4] - 2019-02-20

- Tuned preferences saving snippet.

## [1.20.3] - 2019-02-19

- Added BardCanvas Mobile support on the preferences editor.

## [1.20.2] - 2019-02-15

- Added missing page tags on devices, profile editor and preferences pages.

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
