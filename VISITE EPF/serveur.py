#!/usr/bin/env python3
"""
Serveur HTTP simple pour la visite virtuelle EPF Africa
Permet l'accès multi-appareils sur le réseau local
"""

import http.server
import socketserver
import socket
import webbrowser
import os
import threading
from datetime import datetime

# ==================== CONFIGURATION ====================
PORT = 8000
MAX_CONNECTIONS = 20
# =======================================================

# Compteur de connexions actives
active_connections = 0
connections_lock = threading.Lock()

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    """Version personnalisée avec logs colorés et quota de connexions"""
    
    def handle(self):
        """Vérifie le quota avant de traiter la requête"""
        global active_connections
        
        with connections_lock:
            if active_connections >= MAX_CONNECTIONS:
                # Refuser la connexion si quota atteint
                try:
                    self.connection.sendall(
                        b"HTTP/1.1 503 Service Unavailable\r\n"
                        b"Content-Type: text/html; charset=utf-8\r\n"
                        b"Connection: close\r\n\r\n"
                        b"<h1>Serveur complet</h1>"
                        b"<p>Le nombre maximum de connexions (20) est atteint. Veuillez r\xc3\xa9essayer dans quelques instants.</p>"
                    )
                except:
                    pass
                timestamp = datetime.now().strftime("%H:%M:%S")
                print(f"[{timestamp}] ⛔ Connexion refusée — quota atteint ({active_connections}/{MAX_CONNECTIONS})")
                return
            active_connections += 1
            timestamp = datetime.now().strftime("%H:%M:%S")
            print(f"[{timestamp}] 🟢 Connexion acceptée ({active_connections}/{MAX_CONNECTIONS})")
        
        try:
            super().handle()
        finally:
            with connections_lock:
                active_connections -= 1
                timestamp = datetime.now().strftime("%H:%M:%S")
                print(f"[{timestamp}] 🔴 Connexion fermée ({active_connections}/{MAX_CONNECTIONS})")

    def log_message(self, format, *args):
        """Affiche les logs avec timestamp"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        print(f"[{timestamp}] {format % args}")
    
    def end_headers(self):
        """Ajoute les en-têtes CORS pour éviter les problèmes"""
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()

def get_local_ip():
    """Récupère l'IP locale du PC sur le réseau"""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
        s.close()
        return ip
    except Exception:
        return "127.0.0.1"

def main():
    local_ip = get_local_ip()
    
    print("\n" + "="*60)
    print("  SERVEUR VISITE VIRTUELLE - EPF AFRICA")
    print("="*60)
    print(f"\n Dossier servi : {os.getcwd()}")
    print(f" Port : {PORT}")
    print(f" Quota : {MAX_CONNECTIONS} connexions simultanées max")
    print(f"\n ACCÈS LOCAL :")
    print(f"   → http://localhost:{PORT}")
    print(f"   → http://127.0.0.1:{PORT}")
    print(f"\n📱 ACCÈS MULTI-APPAREILS (sur le même WiFi) :")
    print(f"   → http://{local_ip}:{PORT}")
    print("\n" + "-"*60)
    print("="*60 + "\n")
    
    webbrowser.open(f"http://localhost:{PORT}")
    
    # ThreadingTCPServer pour gérer plusieurs connexions en parallèle
    handler = CustomHTTPRequestHandler
    with socketserver.ThreadingTCPServer(("0.0.0.0", PORT), handler) as httpd:
        print(f"✅ Serveur en cours d'exécution...\n")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n\n🛑 Serveur arrêté par l'utilisateur")
            httpd.shutdown()

if __name__ == "__main__":
    main()