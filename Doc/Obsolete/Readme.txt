                   Iter Vehemens ad Necem v. 0.231 alpha

                     (eli V�kivaltainen Tie Kuolemaan)

            Copyright (C) Fatal Error Productions (FEP) 1999-2001

----------------------------------------------------------------------------

                     Dokumentaatio betatestaajia varten

----------------------------------------------------------------------------

Aluksi
------

Kiitos, ett� vaivauduit imuroimaan IVAN:n. Jos et viel� tied�, millainen
peli sinua odottaa, paljastamme kyseess� olevan uuden Rogue-tyylisen pelin,
eli keinotekoisen maailman, jossa pelaaja liikkuu pitkin joka kerta
uudelleen luotavaa syv�� luolastoa piesten hirvi�armaadoja, joita
tulee matkalla vastaan. Jos olet pelannut Nethackia tai ADOM:ia aiemmin,
tied�t varmasti mist� on kyse.

Olemme FEP:ss� k�ytt�neet paljon aikaa t�m�n teoksen tuottamiseen, ja
luulemme version olevan jo kohtuullisen nautittavaa pelattavaa. Siksi
toivomme, ett� testaisit peli� kohtuullisen tuokion ja kertoisit meille
mielipiteesi. Tarvitsemme palautetta siit�, mik� on jo hyv�� ja toisaalta
siit� mit� pit�isi parantaa, ideoita uusista ominaisuuksista, joita peliin
olisi syyt� lis�t�, ja tietoa mahdollisista virheist� peliss�.
J�lkimm�isist� kaikkein pahimpia ovat sellaiset, jotka kaatavat pelin,
tekev�t siit� l�pipeluukelvottoman tai p�invastoin antavat mahdollisuuden
tarkoituksettoman helppoon pelaajan ominaisuuksien boostaamiseen. Ilmoita
HETI jos sellaisia l�ytyy!

T�RKEINT� KAIKESSA!
-------------------

Kun olet pelannut tarpeeksi, l�het� palautteesi ja l�yt�miesi bugien lista
jompaankumpaan seuraavista osoitteista:

sonor@phpoint.fi
Timo Kiviluoto, IVAN-ohjelmoijatiimin p��organisaattori

hejosa@mantta.fi
Heikki Sairanen, varaorganisaattori ja markkinointijohtaja

----------------------------------------------------------------------------

Laitteistovaatimukset
---------------------

Teoreettinen v�himm�isvaatimus on PC ja sen sis�ll� i386-prosessori tai
uudempi. Todellisuudessa t�ysin j�kkim�tt�m��n kokemukseen tarvitset ainakin
300 MHz:n h�rvelin ja jonkun hitusen j�rkev�mm�n tyypinkin sille. T�m� on
kenties liikaa Rogue likelle, mutta t�ss� vaiheessa emme ole katsoneet
optimointia p��prioriteetiksi.

Koska p��ohjelmoijallamme meni ammoin guruus p��h�n ja h�n vaati saada
tehd� itse kaikki projektin grafiikkafunktiot konekielell�,
n�yt�nohjaintuessa voi ilmet� ongelmia korteissa, jotka eiv�t ole
yhteensopivia Vesa 2.0:n kanssa tai tue resoluution 800x600 16-bittist�
tilaa. Yhdest�k��n sellaisesta n�yt�nohjaimesta emme tosin ole viel�
kuulleet.

K�ytt�j�rjestelmist� Win95 ja uudemmat sek� DOS 7.0 on testattu toimiviksi.
Tulevissa versioissa my�s Linuxia tullaan tukemaan.

----------------------------------------------------------------------------

Pelaamisesta
------------

Liikkuminen
-----------

IVAN toimii niin kuin suurin osa Rogue likeista, eli pelaajan hahmoa
liikutellaan pitkin luolastoa nuolin�pp�imill� (numpad on paras vaihtoehto).
Jos vastaan tulee hirvi�, sit� voi ly�d� yksinkertaisesti k�velem�ll�
(j�lleen nuolin�pp�imist�) hirvi�t� kohti.

Oven tullessa vastaan sit� p�in voi k�vell� ja ohjelma kysyy avatataanko se.

Lista erikoiskomennoista
------------------------

Peliss� on muita enemm�n tai v�hemm�n hy�dyllisi� funktioita, jotka on
listattu t�h�n. Peliss� listan saa my�s esille, painamalla ?-n�pp�int�.

Huomaa, ett� esim. n�pp�imet 'd' ja 'D' tekev�t t�ysin eri asioita, eli
shiftin tai caps lockin tila on huomioitava!

Komennon nimi (n�pp�in) = Selitys.

"consume" (e) = Sy�/juo itemeit�. Ohjelma kysyy, mink� itemin haluat
k�ytt�� t�h�n tarkoitukseen. My�s suoraan maasta sy�minen onnistuu.

"show inventory" (i) = N�yt� hahmon kantamat itemit.

"pick up" (,) = Nosta tavara maasta. Jos tavaroita on useampia, n�yt�
valikko, josta k�ytt�j� voi valita nostettavan.

"quit" (q) = Lopeta peli. (T�t�h�n et tarvitse eik� niin?)

"drop" (d) = Pudota itemi.

"wield" (w) = Ota ase k�teen.

"go up" (<) = Mene yl�s portaita.

"go down" (>) = Mene alas portaita.

"open" (o) = Avaa ovi tai tavara (esim. can).

"close" (c) = Sulje ovi.

"wear armor" (W) = Pue jonkin vaate tai armori p��lle.

"talk" (t) = Puhu.

"wait" (.) = Odota vuoro, lep��.

"save" (s) = Tallenna peli ja lopeta.

"read" (r) = Lue pergamentti tai joku muu tavara.

"dip" (D) = Dippaa, kasta.

"increase gamma" (g) = Lis�� gammaa. (gamma = melkein valoisuus... ei
ihan mutta melkein). Huom. t�m� toimii ehk� noin joka kolmannella koneella.

"decrease gamma" (G) = �skeisen vastakohta.

"show key layout" (?) = N�yt� n�pp�imet.

"look" (l) = Tarkastele maastoa, monstereita, itemeit�.

"engrave" (E) = Kaiverra maahan. (T�ysin turhaa, mutta hauskaa :)

"pray" (y) = Rukoile jumalilta apua.

"kick" (k) = Potkaise jotakin.

"take screenshot" (S) = Kuvankaappaus, jos haluat jostain syyst� laittaa
kuvan parhaasta pelist�si kotisivullesi tai jotain. 

"offer" (O) = Uhraa. Toimii ainoastaan alttarilla.

"increase software gamma" (f) = V�henn� gammaa softapohjaisesti. Toimii
kaikilla koneilla.

"decrease software gamma" (F) = �skeisen vastakohta.

Huijaajille
-----------

Seuraavat n�pp�imet on tarkoitettu l�hinn� tiettyjen ominaisuuksien
nopeaan testaamiseen tuotannon aikana, ja ne ovat t�ysin turhia normaalille
pelaajalle. Listaamme ne kuitenkin varmuuden vuoksi.

"wizard mode" (X) = Aktivoi huijaus-moodi. Huom! T�m� on peruuttamaton
toiminto, joka samalla ev�� highscore-listalle p��syn.

"raise stats" (R) = Nosta pelaajahahmon atribuutteja.

"lower stats" (T) = Laske pelaajahahmon atribuutteja.

"see whole map" (Y) = N�yt� koko kartta. T�m� on varsin hidasta.

"toggle walk through walls cheat" (U) = Toglaa p��lle seinienl�pik�vely-
huijauksen.

----------------------------------------------------------------------------

Taustaa
-------

IVAN sijoittuu 'Terra Valpuria'-universumiin, uskonnolliseen maailmaan,
jossa jumaluudet ovat eritt�in merkitt�v�ss� asemassa ja papisto mahtava.
Pelaaja on nuori valpuristinoviisi, jonka Suuri Valpurin Ylipappi Perttu
l�hett�� vaaralliselle teht�v�lle tappamaan kauhean Elpurin, Pime�n
Sammakon, ja sen lukuisat pahuudenkyll�st�m�t palvelijat. Tosin peliss�
on my�s vaihtoehtoisia, vaikeampia ja nautinnollisempia voittamistapoja
tarpeeksi mahtaville ja Valpurin arvossa pit�mille... Yrit�p� l�yt�� jokin
niist�; jos siihen pystyt, olet aika kova ivannaaja!

OK, tiedet��n, juoni on pelkk� sis�piirivitsi t�ll� hetkell�, mutta se
tullaan uudistamaan. Keskitty pelin engineen �l�k� v�litt� niin paljoa
oudosta huumoristamme.

----------------------------------------------------------------------------

Selviytymisguide
----------------

Ensi kertaa pelaavalle IVAN saattaa tuntua vaikeahkolta, sill� taktiikat
eroavat monin tavoin muista genren peleist�. Olemme t�h�n koonneet
t�rkeimm�t saann�t alkuun p��semiseksi.

I
Pysy alussa Pertun l�hell�, sill� vanhurskaana valpuristina h�n auttaa
nuorempaansa monin tavoin. Ensimm�inen kentt� on muutenkin paljon helpompi
kuin seuraavat; jos sinulla on ongelmia siell� niin �l� mene alasp�in!

II
Sy� vatsasi t�yteen, mutta �l� enemp��. Mutta muista ruokaillessasi:
Olet valpuristi, etk� saa navastaa.

III
Rukoile kun on tarvis. Hyv�t jumalat ovat alussa hy�dyllisempi� (Atavusta
ja Valpuria lukuunottamatta, he vaativat eritt�in hyvi� suhteita ennen kuin
antavat mit��n). Veniuksen pyh�n tulen, Dulciksen mesmerisoivan laulun ja
Consummon avaruudenmuuntelujen avulla selvi�t helposti piiritystilanteista
ja Seges auttaa jos ruokaa p��see loppumaan. Kaaottisia jumaliakin voit
kokeilla, he antavat joskus suuriakin lahjoja, mutta suuttuessaan ovat
eritt�in vaarallisia.

IV
K�yt� jokaista hirvi�t� vastaan sit� asetta, jota oikeassa el�m�ss�
k�ytt�isit. Et varmasti mene pieksem��n pient� pime�� sammakkoa kahdenk�den
miekalla, eth�n osu mihink��n. Sen sijaan yrit�t rutistaa sen k�sin.
T�m� on eritt�in t�rke�� huomata, sill� joihinkin monsuihin ei alussa kerta
kaikkiaan voi osua isoilla ja painavilla aseilla.

V
Jos olet haavoittunut, �l� l�hde heti berserker-hy�kk�ykseen, vaan odota,
kunnes HP on melko l�hell� maksimia. Muuten saat itsesi varmasti hengilt�.

VI
Jos hirvi� on liian vaikea pala sinulle, juokse! Olet usein nopeampi kuin
vastustajasi. Jos et, toisaalta, ainoa vaihtoehto on eksytys (monsut on
tyhmi� kuin vihannekset) tai teleportaatio taian tai Consummolle rukoilun
avulla.

VI
Jos l�yd�t hyv�n haarniskan, pist� se p��lle heti kun jaksat kantaa sen.
Pudota muita tavaroita, jos se on v�ltt�m�t�nt�.

VII
�l� ikin� ja miss��n tapauksessa taistele burdened-tilassa tai huonommassa.
IVAN:ssa t�st� koituva rangaistus on aivan liian suuri.

VIII
Muista aina expottaa hillitt�m�sti.

----------------------------------------------------------------------------

Hyv�� ja voittoisaa pelaamista, toivottaa FEP!