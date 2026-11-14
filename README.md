# Mars Kolonisi Yaşam Destek Simülatörü

C diliyle geliştirilen bu metin tabanlı simülasyonda oyuncu, Mars'taki bir araştırma istasyonunda görev yapan mühendisi yönetir. Amaç; sağlık, enerji, oksijen, su ve yedek parça kaynaklarını dengeli kullanarak kurtarma ekibi gelene kadar 15 günü tamamlamaktır.

## Gereksinimler

- GCC gibi bir C derleyicisi
- Komut satırı veya terminal

## Derleme ve çalıştırma

**Windows (MSYS2 UCRT64, PowerShell veya Windows Terminal):**

```bash
gcc mars_kolonisi.c -o mars_kolonisi.exe -std=c11 -Wall -Wextra -pedantic
./mars_kolonisi.exe
```

**Linux / macOS:**

```bash
gcc mars_kolonisi.c -o mars_kolonisi -std=c11 -Wall -Wextra -pedantic
./mars_kolonisi
```

## Komutlar

| Komut | İşlev | Açıklama |
|---|---|---|
| `G` | Keşif | 15 enerji ve 10 oksijen harcar. Rastgele sonuçla su, yedek parça bulunabilir veya oyuncu eli boş dönebilir. Yaralanma olasılığı ayrıca kontrol edilir. |
| `O` | Oksijen üretimi | Her 2 birim suyu 3 birim oksijene dönüştürür. İşlenen her 1 birim su için 1 enerji harcanır. Tek miktardaki 1 birim su artakalan olarak depoda kalır. |
| `T` | Tamir | Modül hasarını seviyesine göre azaltır ve yedek parça harcar. |
| `D` | Durum raporu | Tüm kaynakları ve kritik uyarıları gösterir. Tur harcamaz. |
| `U` | Dinlenme | 20 enerji kazandırır ve 5 oksijen tüketir. Oksijen 20'nin altındaysa sağlık artmak yerine azalır. |
| `K` | Kum fırtınası | Zorluk seviyesine göre 3, 5 veya 7 turluk `for` döngüsüyle rastgele olaylar oluşturur. |
| `H` | Hava kilidi | Gün sayısından türetilen harf şifresini en fazla 3 denemede bulmayı gerektirir. Her yanlış deneme 5 enerji harcar. |
| `Q` | Çıkış | Simülasyonu bitirir ve sonuç raporunu yazdırır. |

Komutlar büyük/küçük harfe duyarsızdır. `D` ve tanımsız komutlar tur harcamaz. Diğer tanımlı komutlar, kaynak yetersizliği nedeniyle işlem iptal edilse bile bir komut turu sayılır.

## Zorluk seviyeleri

| Seviye | Sağlık | Enerji | Oksijen | Su | Yedek parça | Modül hasarı | Fırtına turu | Keşif taban eşiği |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Kolay (`K`) | 100 | 100 | 100 | 30 | 5 | 10 | 3 | 55 |
| Normal (`N`) | 100 | 85 | 85 | 22 | 3 | 20 | 5 | 45 |
| Zor (`Z`) | 90 | 70 | 70 | 15 | 2 | 35 | 7 | 35 |

Keşif sırasında enerji 40'ın ve oksijen 30'un üzerindeyse su bulma eşiği 10 artar; aksi durumda 10 azalır. Eşik 5-75 aralığında tutulur. Bir sonraki 20 puanlık aralıkta yedek parça bulunabilir; diğer durumda oyuncu eli boş döner. Yaralanma, zarın 10'dan küçük olması veya modül hasarının 70'in üzerinde olması durumunda gerçekleşir.

## Onarım ve fırtına kuralları

- Hafif hasar (0-30): 1 yedek parça harcanır, hasar 15 azaltılır.
- Orta hasar (31-70): 1 yedek parça harcanır, hasar 25 azaltılır.
- Ağır hasar (71-100): 2 yedek parça harcanır, hasar 35 azaltılır.
- Fırtınada zar 0-24 ise sağlık, 25-49 ise oksijen, 50-74 ise modül hasarı etkilenir; 75-99 aralığında oyuncu zarar görmez.
- Fırtınanın çift numaralı turlarında hasar iki katına çıkar. Hava kilidi mühürlüyse oksijen sızıntısı olayı `continue` ile atlanır.

## Zaman ve bitiş kuralları

- Oyun 1. günde başlar. Tur harcayan her 3 komuttan sonra yeni gün başlar.
- Yeni gün başında 8 oksijen azalır. Modül hasarı 70'in üzerindeyse ayrıca 5 sağlık kaybedilir.
- 15. günün 3 komut turu da tamamlanırsa oyun kazanılır.
- Sağlık veya oksijen sıfıra düştüğünde oyun kaybedilir.
- Sağlık, enerji, oksijen ve modül hasarı 0-100 aralığında tutulur; su ve yedek parça negatif olamaz.
- Rastgele olaylarda `srand(time(NULL))` ve `rand() % 100` kullanılır. Üretilen zar ve karşılaştırma eşiği ekranda gösterilir.

## Kullanılan C yapıları

Projede zorluk ve komut seçimi için `switch-case`, harf duyarsızlığı için fall-through kullanılır. Kaynak ve hasar kontrolleri `if`, `if-else` ve `if-else if-else` yapılarıyla gerçekleştirilir. Mantıksal `&&` ve `||` operatörleri keşif, yaralanma ve durum kontrollerinde kullanılır. Kum fırtınası `for`, hava kilidi şifresi ise sınırlı denemeli `do-while` döngüsüyle çalışır. Fırtına döngüsünde `continue` mühürlü hava kilidinde sızıntıyı atlar; `break` sağlık veya oksijen sıfıra düştüğünde döngüyü bitirir.

## Proje dosyaları

- `mars_kolonisi.c` — uygulamanın C kaynak kodu
- `README.md` — proje açıklaması ve kullanım kılavuzu
- `.gitignore` — derleme çıktılarının Git deposuna eklenmesini önleyen kurallar
