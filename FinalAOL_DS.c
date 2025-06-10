#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#define PASS_OK 0
#define PASS_SHORT 1
#define PASS_NO_SYMBOL 2
#define PASS_NO_DIGIT 3


// Escape sequence untuk membersihkan layar terminal
const char *CLEAR_SCREEN = "\e[1;1H\e[2J";

// Struktur data untuk menyimpan informasi pengguna
typedef struct User {
    char username[50];
    char password[50];
    char role[10];  // user atau admin
} User;

typedef struct {
    int id;
    char nama[50];
    char kategori[20];
    char kondisi[30];
    int harga;
    char status[15];  // "Tersedia" atau "Dipinjam"
} Instrument;

typedef struct Rental {
    char username[50];
    int instrumentId;
    char tglPinjam[11];    // "YYYY-MM-DD"
    char tglKembali[11];   // "YYYY-MM-DD"
    char status[10];       // "Aktif" atau "Selesai"
    struct Rental *next;
} Rental;

typedef struct WaitNode {
    char username[50];
    int instrumentId;
    struct WaitNode *next;
} WaitNode;

// Head of waiting list
WaitNode *waitHead = NULL, *waitTail = NULL;

// Ambil tanggal hari ini
void getToday(char *buf) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buf, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}


int isValidPassword();
void registerUser();
int loginUser();
void userMenu(const char *username);
void adminMenu(const char *username);
int getNextInstrumentId();
void tambahAlatMusik();
void tampilkanDaftarInstrumen();
void editAlatMusik();
void hapusAlatMusik();
int containsIgnoreCase();
void cariAlatMusik();
void enqueueWait();
WaitNode *dequeueWaitFor();
int daysBetween();
void addDays();
void sewaAlatMusik(const char*);
void kembalikanAlatMusik(const char*);
void tampilkanTanggal();
void cekNotifikasi(const char *username);
void laporanTransaksi();
void tampilkanAntrian(int id);
void tampilkanSemuaAntrian();
void hapusDariAntrian();


void pause() {
    printf("\nTekan enter untuk melanjutkan...");
    getchar();
}


// Fungsi utama
int main() {
    int pilihan;
    char loggedInUser[50];
    char loggedInRole[10];

    while (1) {
        printf("%s", CLEAR_SCREEN);
        tampilkanTanggal();
        printf("__________.__             __  .__              __________               __         .__          \n");
		printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
		printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
		printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
		printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
		printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
        printf("\n=== MENU UTAMA ===\n");
        printf("1. User\n");
        printf("2. Admin\n");
        printf("3. Keluar\n");
        printf("Pilihan: ");
        scanf("%d", &pilihan);

        switch (pilihan) {
            case 1:
                // Login sebagai user
                if (loginUser(loggedInUser, loggedInRole, "user")) {
				userMenu(loggedInUser);
    			}
                break;
            case 2:
                // Login sebagai admin
                if (loginUser(loggedInUser, loggedInRole, "admin")) {
				adminMenu(loggedInUser);
   				}
                break;
            case 3:
                // Keluar dari program
                printf("Keluar dari program.\n");
                exit(0);
            default:
                printf("Pilihan tidak valid!\n");
                getchar(); getchar();
        }
    }

    return 0;
}

// Fungsi untuk memvalidasi password sesuai kriteria:
// minimal 8 karakter, mengandung simbol, dan angka
int isValidPassword(char password[]) {
    int length = strlen(password);
    int hasSymbol = 0, hasDigit = 0;
    int i;

    if (length < 8) return PASS_SHORT;

    for (i = 0; i < length; i++) {
        if (!isalnum(password[i])) hasSymbol = 1;
        if (isdigit(password[i])) hasDigit = 1;
    }

    if (!hasSymbol) return PASS_NO_SYMBOL;
    if (!hasDigit) return PASS_NO_DIGIT;

    return PASS_OK;
}

// Fungsi untuk registrasi pengguna baru
void registerUser(const char *role) {
    FILE *fp;
    User newUser;
    int exists, check;

    while (1) {
        printf("%s", CLEAR_SCREEN);
        tampilkanTanggal();
        printf("__________.__             __  .__              __________               __         .__          \n");
		printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
		printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
		printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
		printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
		printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
        exists = 0;
        printf("\n====== REGISTRASI (%s) ======\n", role);
        printf("Username: ");
        scanf("%s", newUser.username);

        // Cek apakah username sudah digunakan
        fp = fopen("users.txt", "r");
        if (fp != NULL) {
            User tempUser;
            while (fscanf(fp, "%[^,],%[^,],%s\n", tempUser.username, tempUser.password, tempUser.role) != EOF) {
                if (strcmp(tempUser.username, newUser.username) == 0) {
                    exists = 1;
                    break;
                }
            }
            fclose(fp);
        }

        if (exists) {
            printf("Username sudah terdaftar! Silakan coba lagi.");
            pause();
            continue;
        }

        // Input dan validasi password
		printf("Password harus memiliki:\n");
		printf("- minimal 8 karakter\n");
		printf("- minimal 1 angka\n");
		printf("- minimal 1 simbol\n");
		printf("Password: ");
		scanf("%s", newUser.password);

		check = isValidPassword(newUser.password);
		if (check == PASS_SHORT) {
    		printf("Password minimal 8 karakter!");
    		pause(); continue;
		} else if (check == PASS_NO_SYMBOL) {
    		printf("Password harus mengandung simbol!");
    		pause(); continue;
		} else if (check == PASS_NO_DIGIT) {
    		printf("Password harus mengandung angka!");
			pause(); continue;
		}

        // Simpan role user
        strcpy(newUser.role, role);

        // Tulis data user ke file
        fp = fopen("users.txt", "a");
        if (fp == NULL) {
            printf("Gagal membuka file!\n");
            return;
        }

        fprintf(fp, "%s,%s,%s\n", newUser.username, newUser.password, newUser.role);
        fclose(fp);

        printf("Registrasi berhasil! Silakan login.\n");
        pause();
        break;
    }
}

// Fungsi untuk login pengguna
int loginUser(char *loggedInUsername, char *loggedInRole, const char *expectedRole) {
    char username[50], password[50];
    FILE *fp;
    User tempUser;
    int foundUsername = 0;

    while (1) {
        printf("%s", CLEAR_SCREEN);
        tampilkanTanggal();
        printf("__________.__             __  .__              __________               __         .__          \n");
		printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
		printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
		printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
		printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
		printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
        printf("\n=== LOGIN (%s) ===\n", expectedRole);
        printf("Username: ");
        scanf("%s", username);
        printf("Password: ");
        scanf("%s", password);

        // Buka file dan cek data
        fp = fopen("users.txt", "r");
        if (fp == NULL) {
            printf("Gagal membuka file users.txt\n");
            return 0;
        }

        // Cari user yang cocok
        while (fscanf(fp, "%[^,],%[^,],%s\n", tempUser.username, tempUser.password, tempUser.role) != EOF) {
            if (strcmp(tempUser.username, username) == 0 && strcmp(tempUser.role, expectedRole) == 0) {
                foundUsername = 1;
                if (strcmp(tempUser.password, password) == 0) {
                    // Simpan user yang berhasil login
                    strcpy(loggedInUsername, tempUser.username);
                    strcpy(loggedInRole, tempUser.role);
                    fclose(fp);
                    printf("Login berhasil sebagai %s!\n", loggedInRole);
                    pause();
                    return 1;
                } else {
                    printf("Password salah! Coba lagi.\n");
                    fclose(fp);
                    pause();
                    break;
                }
            }
        }

        // Jika username tidak ditemukan
        if (!foundUsername) {
            printf("Username salah atau bukan %s!\n", expectedRole);
            fclose(fp);
            pause();
        }

        // Tawari registrasi jika belum punya akun
        char choice;
        printf("Belum punya akun? [(y)es/(n)o]: ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            registerUser(expectedRole);
        }
    }
}

void userMenu(const char *username) {
    int choice;
    while (1) {
        printf("%s", CLEAR_SCREEN);
        tampilkanTanggal();
        printf("__________.__             __  .__              __________               __         .__          \n");
		printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
		printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
		printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
		printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
		printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
        printf("========== RENTAL ALAT MUSIK ==========\n");
        printf("Halo %s!\n", username);
        cekNotifikasi(username);
        printf("1. Lihat Daftar Alat Musik\n");
        printf("2. Cari Alat Musik\n");
        printf("3. Sewa Alat Musik\n");
        printf("4. Cek Status Sewa\n");
        printf("5. Kembalikan Alat Musik\n");
        printf("6. Lihat Ulasan & Rating\n");
        printf("7. Logout\n");
        printf("=======================================\n");
        printf("Pilihan: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                tampilkanDaftarInstrumen();
                break;
            case 2:
                cariAlatMusik();
                break;
            case 3:
                sewaAlatMusik(username);
                break;
            case 4:
                printf("Menampilkan status sewa...\n");
                break;
            case 5:
                kembalikanAlatMusik(username);
                break;
            case 6:
                printf("Menampilkan ulasan dan rating...\n");
                break;
            case 7:
                return; // Kembali ke menu utama
            default:
                printf("Pilihan tidak valid!\n");
        }
        getchar(); getchar(); // Pause
    }
}

void adminMenu(const char *username) {
    int choice;
    while (1) {
        printf("%s", CLEAR_SCREEN);
        tampilkanTanggal();
        printf("__________.__             __  .__              __________               __         .__          \n");
		printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
		printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
		printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
		printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
		printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
        printf("========== DASHBOARD ADMIN ==========\n");
        printf("Halo %s!\n", username);
        printf("1. Tambah Alat Musik\n");
        printf("2. Hapus / Edit Alat Musik\n");
        printf("3. Lihat Daftar Peminjaman\n");
        printf("4. Konfirmasi Pengembalian\n");
        printf("5. Lihat Laporan Transaksi\n");
        printf("6. Logout\n");
        printf("7. Manajemen Antrian\n");
        printf("=====================================\n");
        printf("Pilihan: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                tambahAlatMusik();
                break;
            case 2:
                printf("1. Edit Alat Musik\n");
                printf("2. Hapus Alat Musik\n");
                printf("Pilihan: ");
                int subChoice;
                scanf("%d", &subChoice);
                if (subChoice == 1) editAlatMusik();
                else if (subChoice == 2) hapusAlatMusik();
                else printf("Pilihan tidak valid!\n");
                break;
            case 3:
                printf("Menampilkan daftar peminjaman...\n");
                break;
            case 4:
                printf("Mengonfirmasi pengembalian...\n");
                break;
            case 5:
                laporanTransaksi();
                break;
            case 6:
                return; // Kembali ke menu utama
            case 7:
                printf("1. Lihat Antrian per Alat Musik\n");
                printf("2. Lihat Semua Antrian\n");
                printf("3. Hapus User dari Antrian\n");
                printf("Pilihan: ");
                int p;
                scanf("%d", &p);
                if (p == 1) {
                    int id;
                    printf("Masukkan ID alat musik: ");
                    scanf("%d", &id);
                    tampilkanAntrian(id);
                } else if (p == 2) {
                    tampilkanSemuaAntrian();
                } else if (p == 3) {
                    hapusDariAntrian();
                } else {
                    printf("Pilihan tidak valid.\n");
                }
                break;
            default:
                printf("Pilihan tidak valid!\n");
        }
        getchar(); getchar(); // Pause
    }
}

int getNextInstrumentId() {
    FILE *fp = fopen("instruments.txt", "r");
    if (!fp) return 1; // Jika file belum ada, mulai dari ID 1

    Instrument alat;
    int lastId = 0;

    while (fscanf(fp, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n",
                  &alat.id, alat.nama, alat.kategori, alat.kondisi,
                  &alat.harga, alat.status) != EOF) {
        if (alat.id > lastId) lastId = alat.id;
    }

    fclose(fp);
    return lastId + 1; // ID baru = ID terakhir + 1
}

void tambahAlatMusik() {
    tampilkanDaftarInstrumen();
    FILE *fp = fopen("instruments.txt", "a");
    if (!fp) {
        printf("Gagal membuka file!\n");
        getchar(); getchar();
        return;
    }

    Instrument alat;
    alat.id = getNextInstrumentId();  // ID otomatis
    printf("\nMenambahkan alat musik baru dengan ID: %d\n", alat.id);
    printf("Nama Alat Musik: ");
    scanf(" %[^\n]", alat.nama);
    printf("Kategori (Petik/Tiup/Perkusi/Keyboard & Piano/Sound System): ");
    scanf(" %[^\n]", alat.kategori);
    printf("Kondisi (Baru/Bekas/Rusak ringan/Perlu perbaikan): ");
    scanf(" %[^\n]", alat.kondisi);
    printf("Harga Sewa per Hari (dalam angka): ");
    scanf("%d", &alat.harga);
    strcpy(alat.status, "Tersedia");

    fprintf(fp, "%d,%s,%s,%s,%d,%s\n",
            alat.id, alat.nama, alat.kategori, alat.kondisi,
            alat.harga, alat.status);

    fclose(fp);
    printf("Alat musik berhasil ditambahkan!\n");
    pause();
}


void tampilkanDaftarInstrumen() {
    FILE *fp = fopen("instruments.txt", "r");
    if (!fp) {
        printf("Belum ada data alat musik.\n");
        return;
    }

    Instrument alat;
    printf("%s", CLEAR_SCREEN);
    tampilkanTanggal();
    printf("__________.__             __  .__              __________               __         .__          \n");
	printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
	printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
	printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
	printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
	printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
    printf("\n=== DAFTAR ALAT MUSIK ===\n");
    while (fscanf(fp, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n", &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
        printf("ID: %d | Nama: %s | Kategori: %s | Kondisi: %s | Harga: Rp%d | Status: %s\n", alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, alat.status);
    }
    fclose(fp);
    printf("===========================\n");
}


void editAlatMusik() {
	tampilkanDaftarInstrumen();
    FILE *fp = fopen("instruments.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Gagal membuka file!\n");
        return;
    }

    int editId;
    Instrument alat;
    int found = 0;
    printf("Masukkan ID alat musik yang ingin diedit: ");
    scanf("%d", &editId);

    while (fscanf(fp, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n",
              &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
    	if (alat.id == editId) {
        	found = 1;
        	printf("Masukkan data baru:\n");
        	printf("Nama: "); scanf(" %[^\n]", alat.nama);
        	printf("Kategori: "); scanf(" %[^\n]", alat.kategori);
			printf("Kondisi: "); scanf(" %[^\n]", alat.kondisi);
        	printf("Harga Sewa per Hari: "); scanf("%d", &alat.harga);
        	printf("Status (Tersedia/Dipinjam): "); scanf(" %[^\n]", alat.status);
		}
    fprintf(temp, "%d,%s,%s,%s,%d,%s\n",alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, alat.status);
	}


    fclose(fp);
    fclose(temp);
    remove("instruments.txt");
    rename("temp.txt", "instruments.txt");

    if (found) printf("Data berhasil diperbarui!\n");
    else printf("Alat musik tidak ditemukan.\n");
    
    pause();
}

void hapusAlatMusik() {
	tampilkanDaftarInstrumen();
    FILE *fp = fopen("instruments.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!fp || !temp) {
        printf("Gagal membuka file!\n");
        return;
    }

    int deleteId;
    Instrument alat;
    int found = 0;
    printf("Masukkan ID alat musik yang ingin dihapus: ");
    scanf("%d", &deleteId);

    while (fscanf(fp, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n", &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
    		if (alat.id != deleteId) {
        		fprintf(temp, "%d,%s,%s,%s,%d,%s\n", alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, alat.status);
    		} else {
        		found = 1;
    		}
	}

    fclose(fp);
    fclose(temp);
    remove("instruments.txt");
    rename("temp.txt", "instruments.txt");

    if (found) printf("Alat musik berhasil dihapus!\n");
    else printf("Alat musik tidak ditemukan.\n");

    pause();
}

// Fungsi untuk membandingkan dua string apakah str2 ada di dalam str1 (tanpa memperhatikan huruf besar kecil)
int containsIgnoreCase(const char *str1, const char *str2) {
    char s1[1000], s2[1000];
    int i;

    // Salin dan ubah ke huruf kecil
    for (i = 0; str1[i] && i < sizeof(s1) - 1; i++)
        s1[i] = tolower((unsigned char)str1[i]);
    s1[i] = '\0';

    for (i = 0; str2[i] && i < sizeof(s2) - 1; i++)
        s2[i] = tolower((unsigned char)str2[i]);
    s2[i] = '\0';

    // Cari s2 di dalam s1
    return strstr(s1, s2) != NULL;
}

void cariAlatMusik() {
    FILE *fp = fopen("instruments.txt", "r");
    if (!fp) {
        printf("Tidak dapat membuka file instruments.txt\n");
        pause();
        return;
    }

    int pilihan;
    char keyword[50];
    int hargaMax;
    Instrument alat;
    int ditemukan = 0;
	
	printf("%s", CLEAR_SCREEN);
	tampilkanTanggal();
    printf("__________.__             __  .__              __________               __         .__          \n");
	printf("\\______   \\  |__ ___.__._/  |_|  |__   _____   \\______   \\ ____   _____/  |______  |  |   ______\n");
	printf(" |       _/  |  <   |  |\\   __\\  |  \\ /     \\   |       _// __ \\ /    \\   __\\__  \\ |  |  /  ___/\n");
	printf(" |    |   \\   Y  \\___  | |  | |   Y  \\  Y Y  \\  |    |   \\  ___/|   |  \\  |  / __ \\|  |__\\___ \\ \n");
	printf(" |____|_  /___|  / ____| |__| |___|  /__|_|  /  |____|_  /\\___  >___|  /__| (____  /____/____  >\n");
	printf("        \\/     \\/\\/                \\/      \\/          \\/     \\/     \\/          \\/          \\/ \n");
    printf("\n=== PENCARIAN / FILTER ALAT MUSIK ===\n");
    printf("1. Berdasarkan Nama\n");
    printf("2. Berdasarkan Kategori\n");
    printf("3. Berdasarkan Harga Maksimal\n");
    printf("4. Berdasarkan Status (Tersedia/Dipinjam)\n");
    printf("Pilihan: ");
    scanf("%d", &pilihan);
    getchar(); // Membersihkan newline

    switch (pilihan) {
        case 1:
            printf("Masukkan nama (kata kunci): ");
            scanf(" %[^\n]", keyword);
            break;
        case 2:
            printf("Masukkan kategori (Petik/Tiup/Perkusi/...): ");
            scanf(" %[^\n]", keyword);
            break;
        case 3:
            printf("Masukkan harga maksimal: ");
            scanf("%d", &hargaMax);
            break;
        case 4:
            printf("Masukkan status (Tersedia/Dipinjam): ");
            scanf(" %[^\n]", keyword);
            break;
        default:
            printf("Pilihan tidak valid!\n");
            fclose(fp);
            pause();
            return;
    }

    printf("\n=== HASIL PENCARIAN ===\n");
    while (fscanf(fp, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n", &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
        int cocok = 0;
        switch (pilihan) {
            case 1:
                if (containsIgnoreCase(alat.nama, keyword)) cocok = 1;
                break;
            case 2:
                if (strcasecmp(alat.kategori, keyword) == 0) cocok = 1;
                break;
            case 3:
                if (alat.harga <= hargaMax) cocok = 1;
                break;
            case 4:
                if (strcasecmp(alat.status, keyword) == 0) cocok = 1;
                break;
        }
        if (cocok) {
            ditemukan = 1;
            printf("ID: %d | Nama: %s | Kategori: %s | Kondisi: %s | Harga: Rp%d | Status: %s\n", alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, alat.status);
        }
    }

    if (!ditemukan) {
        printf("Tidak ada hasil yang cocok.\n");
    }

    fclose(fp);
    pause();
}

void enqueueWait(const char *username, int id) {
    WaitNode *n = malloc(sizeof(*n));
    strcpy(n->username, username);
    n->instrumentId = id;
    n->next = NULL;
    if (!waitTail) waitHead = n;
    else waitTail->next = n;
    waitTail = n;
}

WaitNode *dequeueWaitFor(int id) {
    WaitNode *prev = NULL, *cur = waitHead;
    while (cur) {
        if (cur->instrumentId == id) {
            if (prev) prev->next = cur->next;
            else waitHead = cur->next;
            if (cur == waitTail) waitTail = prev;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

void addDays(char *result, const char *start, int days) {
    struct tm tm = {0};
    sscanf(start, "%4d-%2d-%2d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;

    time_t t = mktime(&tm) + (days * 24 * 60 * 60);
    struct tm *new_tm = localtime(&t);
    sprintf(result, "%04d-%02d-%02d", new_tm->tm_year + 1900, new_tm->tm_mon + 1, new_tm->tm_mday);
}

void sewaAlatMusik(const char *username) {
    tampilkanDaftarInstrumen(); 
    printf("\nMasukkan ID alat yang ingin disewa: ");
    int id; scanf("%d", &id);

    FILE *f = fopen("instruments.txt","r+");
    Instrument alat; int found = 0;
    long pos;
    while ((pos = ftell(f)) >= 0 && fscanf(f, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n",
           &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
        if (alat.id == id) { found = 1; break; }
    }
    if (!found) { 
        fclose(f); printf("ID tidak ditemukan."); pause(); return; 
    }

    if (strcmp(alat.status,"Tersedia") == 0) {
        // Update status jadi Dipinjam
        fseek(f, pos, SEEK_SET);
        fprintf(f, "%d,%s,%s,%s,%d,%s\n",
                alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, "Dipinjam");
        fclose(f);

        // Simpan ke rentals.txt
        FILE *r = fopen("rentals.txt","a");
        char today[11]; getToday(today);
        char due[11]; addDays(due, today, 7); // Tambahkan 7 hari

        fprintf(r, "%s,%d,%s,%s,%s\n", username, id, today, due, "Aktif");
        fclose(r);

        printf("Sewa berhasil! Harap kembalikan sebelum %s.\n", due);
    } else {
        enqueueWait(username, id);
        printf("Alat sedang dipinjam, Anda masuk waiting list.\n");
    }
    pause();
}


// Hitung selisih hari
int daysBetween(const char *start, const char *end) {
    struct tm a = {0}, b = {0};
    sscanf(start, "%4d-%2d-%2d", &a.tm_year, &a.tm_mon, &a.tm_mday);
    sscanf(end,   "%4d-%2d-%2d", &b.tm_year, &b.tm_mon, &b.tm_mday);
    a.tm_year -= 1900; a.tm_mon -= 1;
    b.tm_year -= 1900; b.tm_mon -= 1;

    time_t t1 = mktime(&a);
    time_t t2 = mktime(&b);
    return (int) difftime(t2, t1) / (60 * 60 * 24);
}

void kembalikanAlatMusik(const char *username) {
    FILE *r = fopen("rentals.txt", "r+");
    Rental temp;
    long pos; int found = 0;
    while ((pos = ftell(r)) >= 0 &&
           fscanf(r, "%[^,],%d,%[^,],%[^,],%s\n",
                  temp.username, &temp.instrumentId, temp.tglPinjam, temp.tglKembali, temp.status) != EOF) {
        if (strcmp(temp.username, username) == 0 && strcmp(temp.status, "Aktif") == 0) {
            found = 1; break;
        }
    }
    if (!found) { fclose(r); printf("Tidak ada sewa aktif.\n"); pause(); return; }

    // Tandai rental selesai
    fseek(r, pos, SEEK_SET);
    fprintf(r, "%s,%d,%s,%s,%s\n",
            temp.username, temp.instrumentId, temp.tglPinjam, temp.tglKembali, "Selesai");
    fclose(r);

    // Update status alat
    FILE *f = fopen("instruments.txt", "r");
    FILE *tmp = fopen("temp.txt", "w");
    Instrument alat;
    while (fscanf(f, "%d,%[^,],%[^,],%[^,],%d,%[^\n]\n",
                  &alat.id, alat.nama, alat.kategori, alat.kondisi, &alat.harga, alat.status) != EOF) {
        if (alat.id == temp.instrumentId) {
            char today[11]; getToday(today);
            int late = daysBetween(temp.tglKembali, today);
            if (late > 0) {
                int denda = late * alat.harga * 0.1;
                printf("Anda terlambat %d hari. Denda: Rp%d\n", late, denda);
            }

            WaitNode *wn = dequeueWaitFor(alat.id);
            if (wn) {
                strcpy(alat.status, "Dipinjam");

                FILE *r2 = fopen("rentals.txt", "a");
                char today[11]; getToday(today);
                char due[11]; addDays(due, today, 7);
                fprintf(r2, "%s,%d,%s,%s,%s\n", wn->username, alat.id, today, due, "Aktif");
                fclose(r2);
                printf("Alat langsung dipinjam ke %s!\n", wn->username);
                free(wn);
            } else {
                strcpy(alat.status, "Tersedia");
            }
        }
        fprintf(tmp, "%d,%s,%s,%s,%d,%s\n",
                alat.id, alat.nama, alat.kategori, alat.kondisi, alat.harga, alat.status);
    }
    fclose(f); fclose(tmp);
    remove("instruments.txt");
    rename("temp.txt", "instruments.txt");
    pause();
}


void tampilkanTanggal() {
    time_t now;
    struct tm *timeinfo;
    char buffer[80];

    time(&now);
    timeinfo = localtime(&now);
    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    printf("Tanggal & Waktu Saat Ini: %s\n", buffer);
}

void cekNotifikasi(const char *username) {
    FILE *fp = fopen("rentals.txt", "r");
    if (!fp) return;

    Rental r;
    char today[11];
    getToday(today);
    int adaNotif = 0;

    while (fscanf(fp, "%[^,],%d,%[^,],%[^,],%[^\n]\n",
                  r.username, &r.instrumentId, r.tglPinjam, r.tglKembali, r.status) != EOF) {
        if (strcmp(r.username, username) == 0 && strcmp(r.status, "Aktif") == 0) {
            int sisaHari = daysBetween(today, r.tglKembali);
            if (sisaHari <= 1) {
                if (!adaNotif) {
                    printf("\n🔔 NOTIFIKASI PENGEMBALIAN:\n");
                    adaNotif = 1;
                }
                if (sisaHari < 0)
                    printf("- Anda TELAT mengembalikan alat ID %d (Harusnya %s)\n", r.instrumentId, r.tglKembali);
                else if (sisaHari == 0)
                    printf("- Anda HARUS mengembalikan alat ID %d HARI INI (%s)\n", r.instrumentId, r.tglKembali);
                else if (sisaHari == 1)
                    printf("- Anda perlu kembalikan alat ID %d BESOK (%s)\n", r.instrumentId, r.tglKembali);
            }
        }
    }

    if (adaNotif) {
        printf("\nSilakan segera lakukan pengembalian jika diperlukan.\n");
    }

    fclose(fp);
}


// Struktur tambahan untuk review
typedef struct {
    int instrumentId;
    char username[50];
    int rating; // 1-5
    char komentar[100];
} Review;

// Fungsi untuk menambahkan review
void tambahUlasan(const char *username) {
    FILE *fp = fopen("reviews.txt", "a");
    if (!fp) {
        printf("Gagal membuka reviews.txt\n");
        return;
    }
    Review rev;
    printf("Masukkan ID alat musik yang ingin diberi ulasan: ");
    scanf("%d", &rev.instrumentId);
    getchar();
    printf("Beri rating (1-5): ");
    scanf("%d", &rev.rating);
    getchar();
    printf("Komentar: ");
    fgets(rev.komentar, sizeof(rev.komentar), stdin);
    rev.komentar[strcspn(rev.komentar, "\n")] = 0;

    strcpy(rev.username, username);
    fprintf(fp, "%d,%s,%d,%s\n", rev.instrumentId, rev.username, rev.rating, rev.komentar);
    fclose(fp);
    printf("Ulasan berhasil ditambahkan!\n");
}

// Fungsi untuk melihat ulasan
void lihatUlasan() {
    FILE *fp = fopen("reviews.txt", "r");
    if (!fp) {
        printf("Belum ada ulasan.\n");
        return;
    }
    Review rev;
    printf("\n=== ULASAN & RATING ===\n");
    while (fscanf(fp, "%d,%[^,],%d,%[^\n]\n", &rev.instrumentId, rev.username, &rev.rating, rev.komentar) != EOF) {
        printf("ID: %d | %s beri rating %d/5\nKomentar: %s\n\n", rev.instrumentId, rev.username, rev.rating, rev.komentar);
    }
    fclose(fp);
}

// Fungsi untuk mencetak nota saat pengembalian
void cetakNota(const char *username, int instrumentId, int hargaPerHari, int durasi, int denda) {
    FILE *fp = fopen("nota.txt", "a");
    if (!fp) {
        printf("Gagal membuat nota.\n");
        return;
    }
    int total = hargaPerHari * durasi + denda;
    time_t now = time(NULL);
    char *waktu = ctime(&now);
    waktu[strcspn(waktu, "\n")] = 0;

    fprintf(fp, "Username: %s\nInstrument ID: %d\nDurasi: %d hari\nHarga: Rp%d\nDenda: Rp%d\nTotal: Rp%d\nTanggal: %s\n---------------------------\n",
            username, instrumentId, durasi, hargaPerHari, denda, total, waktu);
    fclose(fp);
    printf("\nNota disimpan ke nota.txt\n");
}


// Fungsi notifikasi pengingat pada login
void cekPengingat(const char *username) {
    FILE *fp = fopen("rentals.txt", "r");
    if (!fp) return;
    char u[50], tglPinjam[11], tglKembali[11], status[10];
    int id;
    char today[11];
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    sprintf(today, "%04d-%02d-%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);

    while (fscanf(fp, "%[^,],%d,%[^,],%[^,],%s\n", u, &id, tglPinjam, tglKembali, status) != EOF) {
        if (strcmp(u, username) == 0 && strcmp(status, "Aktif") == 0) {
            if (strcmp(tglKembali, today) == 0) {
                printf("\n⚠️  Peringatan: Hari ini adalah batas pengembalian alat ID %d!\n", id);
            }
        }
    }
    fclose(fp);
}

void tampilkanAntrian(int idAlat) {
    printf("\n=== ANTRIAN UNTUK ALAT MUSIK ID %d ===\n", idAlat);
    int kosong = 1;
    WaitNode *curr = waitHead;
    int nomor = 1;
    while (curr) {
        if (curr->instrumentId == idAlat) {
            printf("%d. Username: %s\n", nomor++, curr->username);
            kosong = 0;
        }
        curr = curr->next;
    }
    if (kosong) {
        printf("Tidak ada antrian untuk alat musik ini.\n");
    }
    pause();
}

void hapusDariAntrian() {
    char username[50];
    int idAlat;
    printf("Masukkan username yang ingin dihapus dari antrian: ");
    scanf("%s", username);
    printf("Masukkan ID alat musik: ");
    scanf("%d", &idAlat);

    WaitNode *curr = waitHead;
    WaitNode *prev = NULL;
    int found = 0;

    while (curr) {
        if (strcmp(curr->username, username) == 0 && curr->instrumentId == idAlat) {
            found = 1;
            if (prev) prev->next = curr->next;
            else waitHead = curr->next;
            if (curr == waitTail) waitTail = prev;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (found)
        printf("Antrian berhasil dihapus.\n");
    else
        printf("Data antrian tidak ditemukan.\n");

    pause();
}

void tampilkanSemuaAntrian() {
    printf("\n=== SEMUA ANTRIAN YANG TERDAFTAR ===\n");
    if (!waitHead) {
        printf("Tidak ada antrian saat ini.\n");
    } else {
        WaitNode *curr = waitHead;
        int nomor = 1;
        while (curr) {
            printf("%d. Username: %s | ID Alat Musik: %d\n", nomor++, curr->username, curr->instrumentId);
            curr = curr->next;
        }
    }
    pause();
}

void laporanTransaksi() {
    // Statistik dari rentals.txt
    FILE *fp = fopen("rentals.txt", "r");
    if (!fp) {
        printf("Gagal membuka rentals.txt\n");
    } else {
        Rental data[1000];
        int count = 0;

        // Statistik
        int totalTransaksi = 0;
        int sewaPerAlat[1000] = {0};
        int idTerbanyak = -1;
        int maxSewa = 0;

        printf("\n===== LAPORAN TRANSAKSI =====\n");

        while (fscanf(fp, "%[^,],%d,%[^,],%[^,],%[^\n]\n",
                      data[count].username,
                      &data[count].instrumentId,
                      data[count].tglPinjam,
                      data[count].tglKembali,
                      data[count].status) != EOF) {
            printf("User: %s | ID Alat: %d | Pinjam: %s | Kembali: %s | Status: %s\n",
                   data[count].username,
                   data[count].instrumentId,
                   data[count].tglPinjam,
                   data[count].tglKembali,
                   data[count].status);

            sewaPerAlat[data[count].instrumentId]++;
            totalTransaksi++;
            count++;
        }

        fclose(fp);

        printf("\nTotal Transaksi: %d\n", totalTransaksi);

        for (int i = 0; i < 1000; i++) {
            if (sewaPerAlat[i] > maxSewa) {
                maxSewa = sewaPerAlat[i];
                idTerbanyak = i;
            }
        }

        if (idTerbanyak != -1) {
            printf("Alat Musik Paling Populer: ID %d (Disewa %d kali)\n", idTerbanyak, maxSewa);
        }
    }

    // Total Pendapatan dari nota.txt
    FILE *nota = fopen("nota.txt", "r");
    if (!nota) {
        printf("Gagal membuka nota.txt\n");
    } else {
        int totalPendapatan = 0, harga;
        char line[1000], dummy1[100], dummy2[100];

        while (fgets(line, sizeof(line), nota)) {
            if (sscanf(line, "%[^|]|%[^|]|%d", dummy1, dummy2, &harga) == 3) {
                totalPendapatan += harga;
            }
        }
        fclose(nota);
        printf("\nTotal Pendapatan dari Rental: Rp%d\n", totalPendapatan);
    }

    pause();
}
