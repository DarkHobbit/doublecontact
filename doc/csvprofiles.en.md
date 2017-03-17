# Supported SCV profiles list #

## Explay BM50 ##

Has header: yes, 54 columns.
Charset: UTF-16LE with BOM.
Quote cells: yes.

Header fields:
* Title
* First name
* Middle name
* Last name
* Suffix
* Job title
* Company
* Birthday
* SIP address
* Push-to-talk
* Share view
* User ID
* General mobile
* General phone
* General fax
* General video call
* General web address
* General VOIP address
* General P.O.Box
* General extension
* General street
* General postal/ZIP code
* General city
* General state/province
* General country/region
* Home mobile
* Home phone
* Home email
* Home fax
* Home video call
* Home web address
* Home VOIP address
* Home P.O.Box
* Home extension
* Home street
* Home postal/ZIP code
* Home city
* Home state/province
* Home country/region
* Business mobile
* Business phone
* Business email
* Business fax
* Business video call
* Business web address
* Business VOIP address
* Business P.O.Box
* Business extension
* Business street
* Business postal/ZIP code
* Business city
* Business state/province
* Business country/retion
* <empty column>

Notes: TODO: need check, how many columns really read by phone.

## Explay TV240 ##

Has header: yes, 7 columns.
Charset: UTF-16LE (BOM appearance: unknown).
Quote cells: no (I don't known, what will happened, if contact contains commas in name).

Header fields:
* Name
* Number
* Home Number
* Company Name
* E-mail Address
* Office Number
* Fax Number

Notes: In TV240, only name and first phone are used. Other fields are ignored.
TODO: In sample at source site, some commas in header replaced by semicolon; maybe, it's a mistake.
TODO: check BOM presence.
Source: http://4pda.ru/forum/index.php?showtopic=494378&view=findpost&p=30153623 (on Russian)

