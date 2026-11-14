#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

/* Kullanici girdisini satir satir okuyarak hatali sayi girdilerinde takilmayi onler. */
int sayiOku(const char *istem)
{
    char satir[128];
    char *son;
    long deger;

    while (1)
    {
        printf("%s", istem);

        if (fgets(satir, sizeof(satir), stdin) == NULL)
            return 0;

        errno = 0;
        deger = strtol(satir, &son, 10);

        while (isspace((unsigned char)*son))
            son++;

        if (son != satir && *son == '\0' && errno != ERANGE &&
            deger >= INT_MIN && deger <= INT_MAX)
        {
            return (int)deger;
        }

        printf("Gecersiz sayi girdiniz. Lutfen tekrar deneyin.\n");
    }
}

/* Tek karakter okur ve kucuk harfleri buyuk harfe cevirir. */
char karakterOku(const char *istem)
{
    char satir[128];
    int i;

    while (1)
    {
        printf("%s", istem);

        if (fgets(satir, sizeof(satir), stdin) == NULL)
            return '\0';

        for (i = 0; satir[i] != '\0'; i++)
        {
            if (!isspace((unsigned char)satir[i]))
                return (char)toupper((unsigned char)satir[i]);
        }

        printf("Bos giris yaptiniz. Bir karakter girin.\n");
    }
}

int sinirla(int deger)
{
    if (deger < 0)
        return 0;
    if (deger > 100)
        return 100;
    return deger;
}

/* Her komuttan sonra temel kaynaklarin izin verilen aralikta kalmasini saglar. */
void kaynaklariSinirla(int *saglik, int *enerji, int *oksijen,
                       int *su, int *yedekParca, int *modulHasari)
{
    *saglik = sinirla(*saglik);
    *enerji = sinirla(*enerji);
    *oksijen = sinirla(*oksijen);
    *modulHasari = sinirla(*modulHasari);

    if (*su < 0)
        *su = 0;
    if (*yedekParca < 0)
        *yedekParca = 0;
}

void durumRaporu(int saglik, int enerji, int oksijen, int su,
                 int yedekParca, int modulHasari, int kilitMuhurlu,
                 int gun, int turSayisi)
{
    printf("\n========== DURUM RAPORU ==========\n");
    printf("Gun                 : %d / 15\n", gun);
    printf("Tamamlanan komut    : %d\n", turSayisi);
    printf("Saglik              : %d / 100\n", saglik);
    printf("Enerji              : %d / 100\n", enerji);
    printf("Oksijen             : %d / 100\n", oksijen);
    printf("Su                  : %d birim\n", su);
    printf("Yedek parca         : %d\n", yedekParca);
    printf("Modul hasari        : %d / 100\n", modulHasari);
    printf("Hava kilidi         : %s\n", kilitMuhurlu ? "Muhurli" : "Muhurlenmedi");

    if (oksijen < 25)
        printf("UYARI: Oksijen kritik seviyede!\n");
    if (saglik < 30)
        printf("UYARI: Saglik kritik seviyede!\n");
    if (enerji < 20)
        printf("UYARI: Enerji kritik seviyede!\n");
    if (su < 5)
        printf("UYARI: Su miktari cok az!\n");
    if (yedekParca == 0)
        printf("UYARI: Yedek parca kalmadi!\n");
    if (modulHasari > 70)
        printf("UYARI: Modul agir hasarli!\n");

    if (oksijen >= 25 && saglik >= 30 && enerji >= 20 &&
        su >= 5 && yedekParca > 0 && modulHasari <= 70)
    {
        printf("TUM SISTEMLER NORMAL.\n");
    }

    printf("==================================\n\n");
}

void sonucRaporu(int oyunDurumu, int saglik, int enerji, int oksijen,
                 int su, int yedekParca, int modulHasari, int kilitMuhurlu,
                 int gun, int turSayisi)
{
    int puan = (gun * 100) + saglik + oksijen - modulHasari;

    printf("\n\n========== SIMULASYON SONUCU ==========\n");

    if (oyunDurumu == 1)
        printf("SONUC: KAZANDINIZ! Kurtarma ekibi Mars istasyonuna ulasti.\n");
    else if (oyunDurumu == 2)
        printf("SONUC: KAYBETTINIZ! Yasam destek kosullari sona erdi.\n");
    else
        printf("SONUC: Simulasyondan kendi isteginizle ciktiniz.\n");

    printf("Bulunulan gun       : %d / 15\n", gun);
    printf("Toplam komut turu   : %d\n", turSayisi);
    printf("Son saglik          : %d / 100\n", saglik);
    printf("Son enerji          : %d / 100\n", enerji);
    printf("Son oksijen         : %d / 100\n", oksijen);
    printf("Son su              : %d birim\n", su);
    printf("Son yedek parca     : %d\n", yedekParca);
    printf("Son modul hasari    : %d / 100\n", modulHasari);
    printf("Hava kilidi         : %s\n", kilitMuhurlu ? "Muhurli" : "Muhurlenmedi");
    printf("PUAN = (gun x 100) + saglik + oksijen - modul hasari\n");
    printf("PUANINIZ: %d\n", puan);
    printf("========================================\n");
}

int main(void)
{
    int saglik, enerji, oksijen, su, yedekParca, modulHasari;
    int kilitMuhurlu = 0;
    int gun = 1;
    int turSayisi = 0;
    int oyunDurumu = 0; /* 0: devam, 1: kazanma, 2: kaybetme, 3: cikis */
    int firtinaTurSayisi = 0;
    int kesifTabanEsigi = 45;
    char zorluk;
    char komut;
    int zorlukSecildi = 0;

    srand((unsigned int)time(NULL));

    printf("===============================================\n");
    printf("       MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("===============================================\n");
    printf("Kurtarma ekibi gelene kadar 15 gun hayatta kal!\n\n");

    /* Zorluk secimi SWITCH-CASE ile yapilir; gecersiz secim yeniden istenir. */
    do
    {
        zorluk = karakterOku("Zorluk secin (K: Kolay, N: Normal, Z: Zor): ");

        switch (zorluk)
        {
            case 'K':
                saglik = 100;
                enerji = 100;
                oksijen = 100;
                su = 30;
                yedekParca = 5;
                modulHasari = 10;
                firtinaTurSayisi = 3;
                kesifTabanEsigi = 55;
                zorlukSecildi = 1;
                printf("Kolay seviye secildi. Kaynaklar daha yuksek.\n");
                break;

            case 'N':
                saglik = 100;
                enerji = 85;
                oksijen = 85;
                su = 22;
                yedekParca = 3;
                modulHasari = 20;
                firtinaTurSayisi = 5;
                kesifTabanEsigi = 45;
                zorlukSecildi = 1;
                printf("Normal seviye secildi.\n");
                break;

            case 'Z':
                saglik = 90;
                enerji = 70;
                oksijen = 70;
                su = 15;
                yedekParca = 2;
                modulHasari = 35;
                firtinaTurSayisi = 7;
                kesifTabanEsigi = 35;
                zorlukSecildi = 1;
                printf("Zor seviye secildi. Kaynaklar daha dusuk, riskler daha yuksek.\n");
                break;

            default:
                printf("Gecersiz zorluk seviyesi! K, N veya Z girin.\n");
                break;
        }
    } while (zorlukSecildi == 0);

    kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                      &yedekParca, &modulHasari);

    printf("\nKomutlar: G=Kesif, O=Oksijen, T=Tamir, D=Durum,\n");
    printf("          U=Dinlen, K=Kum firtinasi, H=Hava kilidi, Q=Cikis\n");
    printf("Not: D ve gecersiz komutlar tur harcamaz.\n\n");

    /* Ana komut dinleme dongusu. */
    do
    {
        int turHarcandi = 0;
        int cikisIstendi = 0;

        komut = karakterOku("\nKomut girin: ");

        switch (komut)
        {
            case 'g':
            case 'G':
            {
                int zarKesif;
                turHarcandi = 1;
                int suEsigi;
                int parcaEsigi;
                int zarYaralanma;

                if (enerji < 15)
                {
                    printf("Kesif iptal: Bu komut icin en az 15 enerji gerekli.\n");
                    break;
                }

                enerji -= 15;
                oksijen -= 10;
                printf("Kesif maliyeti: 15 enerji ve 10 oksijen.\n");

                /* Kaynak durumuna gore su bulma esigi yukseltilir veya dusurulur. */
                suEsigi = kesifTabanEsigi;
                if (enerji > 40 && oksijen > 30)
                    suEsigi += 10;
                else
                    suEsigi -= 10;

                if (suEsigi < 5)
                    suEsigi = 5;
                if (suEsigi > 75)
                    suEsigi = 75;

                parcaEsigi = suEsigi + 20;
                if (parcaEsigi > 95)
                    parcaEsigi = 95;

                zarKesif = rand() % 100;
                printf("Kesif zari: %d / Esikler: su < %d, yedek parca < %d\n",
                       zarKesif, suEsigi, parcaEsigi);

                if (zarKesif < suEsigi)
                {
                    su += 8;
                    printf("Kesif sonucu: 8 birim su bulundu.\n");
                }
                else if (zarKesif < parcaEsigi)
                {
                    yedekParca += 1;
                    printf("Kesif sonucu: 1 yedek parca bulundu.\n");
                }
                else
                {
                    printf("Kesif sonucu: Istasyona eli bos donuldu.\n");
                }

                zarYaralanma = rand() % 100;
                printf("Yaralanma zari: %d / Esik: 10 (zar < 10 veya modul hasari > 70)\n",
                       zarYaralanma);

                if (zarYaralanma < 10 || modulHasari > 70)
                {
                    saglik -= 10;
                    printf("Kesif sirasinda yaralandiniz: 10 saglik kaybettiniz.\n");
                }
                else
                {
                    printf("Kesif sirasinda yaralanma olmadi.\n");
                }

                turHarcandi = 1;
                break;
            }

            case 'o':
            case 'O':
            {
                int istenenSu;
                turHarcandi = 1;
                int islenecekSu;
                int enerjiMaliyeti;
                int enFazlaSu;

                if (oksijen >= 100)
                {
                    printf("Oksijen deposu zaten dolu. Uretim yapilamadi.\n");
                    break;
                }
                if (su <= 0)
                {
                    printf("Oksijen uretmek icin suyunuz yok.\n");
                    break;
                }

                istenenSu = sayiOku("Kullanmak istediginiz su miktari: ");
                if (istenenSu <= 0 || istenenSu > su)
                {
                    printf("Gecersiz miktar. 1 ile mevcut su miktari arasinda giris yapin.\n");
                    break;
                }

                /* 2 su -> 3 oksijen. Tek miktardaki 1 birim artik kalir. */
                islenecekSu = istenenSu - (istenenSu % 2);
                enFazlaSu = ((100 - oksijen) / 3) * 2;
                if (islenecekSu > enFazlaSu)
                    islenecekSu = enFazlaSu;

                if (islenecekSu <= 0)
                {
                    printf("Oksijen kapasitesi yeni bir 3 birimlik uretime yetmiyor.\n");
                    break;
                }

                /* Bu uygulamada islenen her 1 su birimi 1 enerji harcar. */
                enerjiMaliyeti = islenecekSu;
                if (enerji < enerjiMaliyeti)
                {
                    printf("Uretim iptal: %d enerji gerekiyor, mevcut enerji %d.\n",
                           enerjiMaliyeti, enerji);
                    break;
                }

                su -= islenecekSu;
                enerji -= enerjiMaliyeti;
                oksijen += (islenecekSu / 2) * 3;

                printf("Uretim tamamlandi: %d su harcandi, %d oksijen uretildi, %d enerji harcandi.\n",
                       islenecekSu, (islenecekSu / 2) * 3, enerjiMaliyeti);
                if (istenenSu % 2 != 0)
                    printf("Tek miktardaki 1 birim su artik olarak depoda kaldi.\n");
                if (islenecekSu < istenenSu - (istenenSu % 2))
                    printf("Oksijen deposu siniri nedeniyle bazi su birimleri kullanilmadi.\n");

                turHarcandi = 1;
                break;
            }

            case 't':
            case 'T':
            {
                int gerekliParca;
                turHarcandi = 1;
                int onarimMiktari;

                if (modulHasari == 0)
                {
                    printf("Modulde onarilmasi gereken hasar yok.\n");
                    break;
                }

                /* IF - ELSE IF - ELSE zinciri hasar kademesini belirler. */
                if (modulHasari <= 30)
                {
                    gerekliParca = 1;
                    onarimMiktari = 15;
                    printf("Hasar seviyesi: HAFIF.\n");
                }
                else if (modulHasari <= 70)
                {
                    gerekliParca = 1;
                    onarimMiktari = 25;
                    printf("Hasar seviyesi: ORTA.\n");
                }
                else
                {
                    gerekliParca = 2;
                    onarimMiktari = 35;
                    printf("Hasar seviyesi: AGIR.\n");
                }

                if (yedekParca < gerekliParca)
                {
                    printf("Tamir iptal: %d yedek parca gerekli, mevcut parca %d.\n",
                           gerekliParca, yedekParca);
                    break;
                }

                yedekParca -= gerekliParca;
                modulHasari -= onarimMiktari;
                printf("Tamir tamamlandi: %d yedek parca harcandi, hasar %d azaltildi.\n",
                       gerekliParca, onarimMiktari);
                turHarcandi = 1;
                break;
            }

            case 'd':
            case 'D':
                durumRaporu(saglik, enerji, oksijen, su, yedekParca,
                            modulHasari, kilitMuhurlu, gun, turSayisi);
                /* Durum raporu zaman/tur harcamaz. */
                break;

            case 'u':
            case 'U':
                enerji += 20;

                /* Oksijen, dinlenme sirasinda da tuketilir. */
                if (oksijen < 20)
                {
                    /* Ic ice IF: Dusuk oksijende saglik artisi yerine kayip uygulanir. */
                    if (saglik > 0)
                    {
                        saglik -= 10;
                        printf("Oksijen 20'nin altinda: dinlenirken 10 saglik kaybettiniz.\n");
                    }
                    else
                    {
                        printf("Saglik zaten sifir.\n");
                    }
                }
                else
                {
                    if (saglik < 100)
                    {
                        saglik += 10;
                        printf("Dinlenme sayesinde 10 saglik kazandiniz.\n");
                    }
                    else
                    {
                        printf("Sagliginiz zaten en yuksek seviyede.\n");
                    }
                }

                oksijen -= 5;
                printf("Enerji 20 artti, oksijen 5 azaldi.\n");
                turHarcandi = 1;
                break;

            case 'k':
            case 'K':
            {
                int i;
                int hasarliTur = 0;
                int toplamSaglikKaybi = 0;
                int toplamOksijenKaybi = 0;

                printf("Kum firtinasi basladi. Sure: %d tur.\n", firtinaTurSayisi);

                /* FOR dongusu her firtina turundeki rastgele olayi simule eder. */
                for (i = 1; i <= firtinaTurSayisi; i++)
                {
                    int zar = rand() % 100;
                    int carpan = (i % 2 == 0) ? 2 : 1;
                    int buTurHasarAldi = 0;

                    printf("\nFirtina turu %d: ", i);

                    if (zar < 25)
                    {
                        int oncekiSaglik = saglik;
                        int kayip = 8 * carpan;
                        printf("Zar: %d / Esik: 25 (zar < 25) -> Can kaybi\n", zar);
                        saglik -= kayip;
                        kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                                          &yedekParca, &modulHasari);
                        toplamSaglikKaybi += oncekiSaglik - saglik;
                        if (oncekiSaglik > saglik)
                            buTurHasarAldi = 1;
                    }
                    else if (zar < 50)
                    {
                        printf("Zar: %d / Esik: 50 (25 <= zar < 50) -> Oksijen sizintisi\n", zar);

                        if (kilitMuhurlu == 1)
                        {
                            printf("Hava kilidi muhurli: Oksijen sizintisi bu tur continue ile atlandi.\n");
                            continue;
                        }

                        {
                            int oncekiOksijen = oksijen;
                            int kayip = 10 * carpan;
                            oksijen -= kayip;
                            kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                                              &yedekParca, &modulHasari);
                            toplamOksijenKaybi += oncekiOksijen - oksijen;
                            if (oncekiOksijen > oksijen)
                                buTurHasarAldi = 1;
                        }
                    }
                    else if (zar < 75)
                    {
                        int oncekiHasar = modulHasari;
                        int kayip = 10 * carpan;
                        printf("Zar: %d / Esik: 75 (50 <= zar < 75) -> Modul hasari\n", zar);
                        modulHasari += kayip;
                        kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                                          &yedekParca, &modulHasari);
                        if (modulHasari > oncekiHasar)
                            buTurHasarAldi = 1;
                    }
                    else
                    {
                        printf("Zar: %d / Esik: 75 (zar >= 75) -> Firtinadan zararsiz korunma\n", zar);
                    }

                    if (buTurHasarAldi)
                        hasarliTur++;

                    if (i % 2 == 0)
                        printf("Cift numarali tur: ruzgar siddeti iki kat, carpan = 2.\n");

                    kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                                      &yedekParca, &modulHasari);

                    if (saglik <= 0 || oksijen <= 0)
                    {
                        printf("Saglik veya oksijen sifira dustu; break ile firtina durduruldu.\n");
                        break;
                    }
                }

                printf("\nFirtina ozeti: Hasar alinan tur = %d, toplam saglik kaybi = %d, toplam oksijen kaybi = %d\n",
                       hasarliTur, toplamSaglikKaybi, toplamOksijenKaybi);
                turHarcandi = 1;
                break;
            }

            case 'h':
            case 'H':
            {
                char sifre;
                turHarcandi = 1;
                char tahmin = '\0';
                int deneme = 0;
                int enerjiYetersiz = 0;

                /* Sifre, gun sayisindan aritmetik olarak turetilir. */
                sifre = (char)('A' + (gun % 26));
                printf("Hava kilidi ipucu: Sifre, A harfinden bugunun gun sayisi kadar ileridedir.\n");
                printf("En fazla 3 deneme hakkiniz var. Her yanlis deneme 5 enerji harcar.\n");

                do
                {
                    if (enerji < 5)
                    {
                        printf("Yetersiz enerji: Yeni bir deneme icin 5 enerji gerekiyor.\n");
                        enerjiYetersiz = 1;
                        break;
                    }

                    tahmin = karakterOku("Tek harfli sifre tahmininiz: ");
                    deneme++;

                    if (tahmin == sifre)
                    {
                        printf("Sifre dogru! Hava kilidi muhurleniyor.\n");
                    }
                    else
                    {
                        enerji -= 5;
                        printf("Yanlis sifre. 5 enerji kaybettiniz.\n");

                        if (tahmin < sifre)
                            printf("Ipucu: Alfabede ileri git.\n");
                        else
                            printf("Ipucu: Alfabede geri git.\n");
                    }
                } while (tahmin != sifre && deneme < 3);

                if (tahmin == sifre)
                {
                    kilitMuhurlu = 1;
                }
                else if (deneme == 3)
                {
                    modulHasari += 20;
                    printf("Uc deneme de basarisiz. Modul hasari 20 artti.\n");
                }

                if (deneme > 0)
                    turHarcandi = 1;

                if (enerjiYetersiz)
                    printf("Enerji yetersizligi nedeniyle sifre denemesi sona erdi.\n");
                break;
            }

            case 'q':
            case 'Q':
                printf("Cikis komutu verildi.\n");
                turHarcandi = 1;
                cikisIstendi = 1;
                break;

            default:
                printf("Bilinmeyen komut! G, O, T, D, U, K, H veya Q girin.\n");
                /* default komutu tur harcatmaz. */
                break;
        }

        /* Degerler komut sonrasinda her zaman izin verilen araliga cekilir. */
        kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                          &yedekParca, &modulHasari);

        if (turHarcandi)
        {
            turSayisi++;
            printf("Komut turu sayaci: %d\n", turSayisi);

            if (cikisIstendi)
            {
                oyunDurumu = 3;
                break;
            }

            /* Her 3 turda yeni gun baslar. 15. gunun 3 turu bitince oyun kazanilir. */
            if (turSayisi % 3 == 0)
            {
                if (saglik <= 0 || oksijen <= 0)
                {
                    oyunDurumu = 2;
                    break;
                }

                if (gun == 15)
                {
                    oyunDurumu = 1;
                    break;
                }

                gun++;
                oksijen -= 8;
                printf("Yeni gun basladi: Gun %d. Gun baslangicinda oksijen 8 azaldi.\n", gun);

                if (modulHasari > 70)
                {
                    saglik -= 5;
                    printf("Modul hasari 70'in ustunde: gun basinda 5 saglik kaybedildi.\n");
                }
            }
        }

        kaynaklariSinirla(&saglik, &enerji, &oksijen, &su,
                          &yedekParca, &modulHasari);

        /* Her turun sonunda olum kosullari kontrol edilir. */
        if (saglik <= 0 || oksijen <= 0)
        {
            oyunDurumu = 2;
            break;
        }

    } while (oyunDurumu == 0);

    sonucRaporu(oyunDurumu, saglik, enerji, oksijen, su, yedekParca,
                modulHasari, kilitMuhurlu, gun, turSayisi);

    return 0;
}
