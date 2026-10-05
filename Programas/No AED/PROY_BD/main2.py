'''PROYECTO FINAL BASE DE DATOS II
INTEGRANTES:
- SAMUEL ALEXANDER IMÁN QUISPE
- PAOLO JESUS MOSTAJO ALOR
- ESTEBAN ANDRÉS MEDINA CHINO'''

import os
import csv
from collections import defaultdict
import tkinter as tk
from tkinter import ttk, messagebox, filedialog

# *** Clases relacionadas al Disco ***

class Sector:
    def __init__(self, capacidad=64):
        self.capacidad = capacidad
        self.ocupado = 0
        self.datos = []

    def almacenar_fragmento(self, dato, offset):
        espacio_disponible = self.capacidad - self.ocupado
        bytes_a_guardar = min(espacio_disponible, len(dato) - offset)
        self.datos.append(dato[offset:offset + bytes_a_guardar])
        self.ocupado += bytes_a_guardar
        return bytes_a_guardar

    def mostrar_estado(self):
        return f"({self.ocupado} - {self.capacidad})"


class Pista:
    def __init__(self, num_sectores, capacidad_sector):
        self.sectores = [Sector(capacidad_sector) for _ in range(num_sectores)]


class Superficie:
    def __init__(self, num_pistas, num_sectores, capacidad_sector):
        self.pistas = [Pista(num_sectores, capacidad_sector) for _ in range(num_pistas)]


class Disco:
    def __init__(self, num_discos, num_pistas, num_sectores, capacidad_sector):
        self.num_discos = num_discos
        self.num_superficies = 2
        self.discos = [
            [Superficie(num_pistas, num_sectores, capacidad_sector) for _ in range(self.num_superficies)]
            for _ in range(num_discos)
        ]
        self.indice = defaultdict(list)

    def almacenar_datos(self, datos):
        disco_idx = superficie_idx = pista_idx = sector_idx = 0

        for dato, size in datos:
            offset = 0
            while offset < size:
                sector = self.discos[disco_idx][superficie_idx].pistas[pista_idx].sectores[sector_idx]
                almacenado = sector.almacenar_fragmento(dato, offset)
                self.indice[dato].append((disco_idx + 1, pista_idx + 1, sector_idx + 1, almacenado))
                offset += almacenado

                if sector.ocupado == sector.capacidad:
                    sector_idx += 1
                    if sector_idx >= len(self.discos[disco_idx][superficie_idx].pistas[pista_idx].sectores):
                        sector_idx = 0
                        pista_idx += 1
                        if pista_idx >= len(self.discos[disco_idx][superficie_idx].pistas):
                            pista_idx = 0
                            superficie_idx += 1
                            if superficie_idx >= self.num_superficies:
                                superficie_idx = 0
                                disco_idx += 1
                                if disco_idx >= self.num_discos:
                                    print("Error: No hay más espacio en los discos.")
                                    return

    def mostrar_estructura(self):
        for d_idx, disco in enumerate(self.discos):
            print(f"Disco {d_idx + 1}:")
            for s_idx, superficie in enumerate(disco):
                print(f"  Superficie {s_idx + 1}:")
                for p_idx, pista in enumerate(superficie.pistas):
                    print(f"    Pista {p_idx + 1}:")
                    for k, sector in enumerate(pista.sectores):
                        print(f"      Sector {k + 1} {sector.mostrar_estado()}")

    @staticmethod
    def to_lower(s):
        return s.lower()

# *** Funciones para manejar archivos ***

def extraer_datos_csv(filename):
    if not os.path.isfile(filename):
        print(f"Error: El archivo '{filename}' no se pudo encontrar.")
        return []

    datos = []
    try:
        with open(filename, mode='r', encoding='utf-8') as file:
            reader = csv.reader(file)
            next(reader)
            for row in reader:
                for value in row:
                    datos.append((value, len(value)))
    except Exception as e:
        print(f"Error al abrir el archivo: {e}")
        return []

    return datos

def analizar_estructura_txt(filename):
    estructura = []
    try:
        with open(filename, mode='r', encoding='utf-8') as file:
            for line in file:
                line = line.strip().upper()
                if line.startswith("CREATE TABLE"):
                    continue
                if "(" in line or ")" in line or line == ";":
                    continue

                partes = line.split()
                nombre = partes[0]
                tipo = partes[1]

                # Eliminar restricciones como PRIMARY KEY o NOT NULL
                if "(" in tipo:
                    tipo = tipo.split("(")[0]

                if "INT" in tipo:
                    tamanio = "4 bytes"
                elif "CHAR" in tipo or "TEXT" in tipo or "VARCHAR" in tipo:
                    tamanio = f"{tipo.split('(')[1].strip(')')} bytes" if "(" in tipo else "Variable"
                elif "DECIMAL" in tipo:
                    tamanio = "16 bytes"
                else:
                    tamanio = "Desconocido"

                estructura.append((nombre, tipo, tamanio))

    except Exception as e:
        print(f"Error al analizar el archivo: {e}")

    return estructura

def mostrar_estructura_txt_gui(estructura):
    ventana = tk.Toplevel()
    ventana.title("Estructura del Archivo TXT")
    ventana.geometry("600x400")

    tree = ttk.Treeview(ventana, columns=("Nombre", "Tipo", "Tamaño"), show="headings")
    tree.heading("Nombre", text="Nombre")
    tree.heading("Tipo", text="Tipo")
    tree.heading("Tamaño", text="Tamaño")

    for nombre, tipo, tamanio in estructura:
        tree.insert("", tk.END, values=(nombre, tipo, tamanio))

    tree.pack(expand=True, fill=tk.BOTH)

def subir_archivo_txt(disco):
    filename = filedialog.askopenfilename(
        title="Seleccione un archivo TXT",
        filetypes=[("Archivos TXT", "*.txt")]
    )

    if filename:
        estructura = analizar_estructura_txt(filename)
        if estructura:
            mostrar_estructura_txt_gui(estructura)
        else:
            messagebox.showwarning("Advertencia", "No se encontraron datos válidos en el archivo TXT.")

def subir_archivo_csv(disco):
    filename = filedialog.askopenfilename(
        title="Seleccione un archivo CSV",
        filetypes=[("Archivos CSV", "*.csv")]
    )

    if filename:
        datos = extraer_datos_csv(filename)
        if datos:
            disco.almacenar_datos(datos)
            messagebox.showinfo("Éxito", f"Datos del archivo '{os.path.basename(filename)}' cargados correctamente.")
        else:
            messagebox.showwarning("Advertencia", "No se pudieron cargar los datos del archivo CSV.")

def mostrar_estructura_gui(disco):
    ventana = tk.Toplevel()
    ventana.title("Estado del Disco")
    ventana.geometry("600x400")

    text_area = tk.Text(ventana, wrap=tk.WORD, font=("Courier", 10))
    text_area.pack(expand=True, fill=tk.BOTH)

    for d_idx, d in enumerate(disco.discos):
        text_area.insert(tk.END, f"Disco {d_idx + 1}:\n")
        for s_idx, s in enumerate(d):
            text_area.insert(tk.END, f"  Superficie {s_idx + 1}:\n")
            for p_idx, p in enumerate(s.pistas):
                text_area.insert(tk.END, f"    Pista {p_idx + 1}:\n")
                for k, sector in enumerate(p.sectores):
                    text_area.insert(tk.END, f"      Sector {k + 1} {sector.mostrar_estado()}\n")

    text_area.config(state=tk.DISABLED)

def buscar_datos_gui(disco):
    def buscar():
        nombre = entrada_nombre.get()
        resultado.delete(1.0, tk.END)
        encontrado = False

        for dato, posiciones in disco.indice.items():
            if disco.to_lower(dato).startswith(disco.to_lower(nombre)):
                encontrado = True
                resultado.insert(tk.END, f"Dato: {dato}\n")
                for disco_id, pista, sector, size in posiciones:
                    resultado.insert(tk.END, f"  Disco: {disco_id}, Pista: {pista}, Sector: {sector}, Tamaño: {size} bytes\n")

        if not encontrado:
            resultado.insert(tk.END, f"No se encontraron coincidencias para '{nombre}'.\n")

    ventana = tk.Toplevel()
    ventana.title("Buscar Datos por Nombre")
    ventana.geometry("600x400")

    tk.Label(ventana, text="Ingrese el nombre o parte del dato a buscar:").pack(pady=5)
    entrada_nombre = tk.Entry(ventana, width=40)
    entrada_nombre.pack(pady=5)

    tk.Button(ventana, text="Buscar", command=buscar).pack(pady=5)

    resultado = tk.Text(ventana, wrap=tk.WORD, font=("Courier", 10))
    resultado.pack(expand=True, fill=tk.BOTH)

def modificar_configuracion_gui(disco_ref, datos):
    def guardar():
        try:
            num_discos = int(entry_discos.get())
            num_pistas = int(entry_pistas.get())
            num_sectores = int(entry_sectores.get())
            capacidad_sector = int(entry_capacidad.get())

            nuevo_disco = Disco(num_discos, num_pistas, num_sectores, capacidad_sector)
            nuevo_disco.almacenar_datos(datos)

            
            disco_ref[0] = nuevo_disco

            messagebox.showinfo("Éxito", "Configuración actualizada y datos recargados.")
            ventana.destroy()
        except ValueError:
            messagebox.showerror("Error", "Por favor, ingrese valores válidos.")

    ventana = tk.Toplevel()
    ventana.title("Modificar Configuración del Disco")
    ventana.geometry("400x300")

    tk.Label(ventana, text="Número de Discos:").pack(pady=5)
    entry_discos = tk.Entry(ventana)
    entry_discos.insert(0, str(disco_ref[0].num_discos))
    entry_discos.pack(pady=5)

    tk.Label(ventana, text="Número de Pistas:").pack(pady=5)
    entry_pistas = tk.Entry(ventana)
    entry_pistas.insert(0, str(len(disco_ref[0].discos[0][0].pistas)))
    entry_pistas.pack(pady=5)

    tk.Label(ventana, text="Número de Sectores por Pista:").pack(pady=5)
    entry_sectores = tk.Entry(ventana)
    entry_sectores.insert(0, str(len(disco_ref[0].discos[0][0].pistas[0].sectores)))
    entry_sectores.pack(pady=5)

    tk.Label(ventana, text="Capacidad de cada Sector (bytes):").pack(pady=5)
    entry_capacidad = tk.Entry(ventana)
    entry_capacidad.insert(0, str(disco_ref[0].discos[0][0].pistas[0].sectores[0].capacidad))
    entry_capacidad.pack(pady=5)

    tk.Button(ventana, text="Guardar", command=guardar).pack(pady=10)

# Programa principal con Tkinter
def main():
    num_discos, num_pistas, num_sectores, capacidad_sector = 1, 5, 10, 64
    disco_ref = [Disco(num_discos, num_pistas, num_sectores, capacidad_sector)]

    root = tk.Tk()
    root.title("Gestión de Disco")
    root.geometry("400x400")

    tk.Label(root, text="Gestión de Disco", font=("Helvetica", 16)).pack(pady=20)

    tk.Button(root, text="Modificar Configuración", command=lambda: modificar_configuracion_gui(disco_ref, [])).pack(pady=10)
    tk.Button(root, text="Mostrar Estado del Disco", command=lambda: mostrar_estructura_gui(disco_ref[0])).pack(pady=10)
    tk.Button(root, text="Buscar Datos", command=lambda: buscar_datos_gui(disco_ref[0])).pack(pady=10)
    tk.Button(root, text="Subir Archivo CSV", command=lambda: subir_archivo_csv(disco_ref[0])).pack(pady=10)
    tk.Button(root, text="Subir Archivo TXT", command=lambda: subir_archivo_txt(disco_ref[0])).pack(pady=10)
    tk.Button(root, text="Salir", command=root.quit).pack(pady=10)

    root.mainloop()

if __name__ == "__main__":
    main()
