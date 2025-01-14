import tkinter as tk
from tkinter import messagebox
import socket
import threading

class CalendarClientGUI:
    def __init__(self, master):
        self.master = master
        self.master.title("Calendar Client")
        self.master.geometry("800x800")

        self.server_address = ("127.0.0.1", 1337)
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            self.client_socket.connect(self.server_address)
        except Exception as e:
            messagebox.showerror("Connection Error", f"Could not connect to server: {e}")
            self.master.destroy()
            return

        self.create_widgets()

    def create_widgets(self):
        tk.Label(self.master, text="Create User").pack(pady=5)
        self.username_entry = tk.Entry(self.master, width=30)
        self.username_entry.pack(pady=5)
        self.username_entry.insert(0, "Enter username")
        self.username_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(self.username_entry, "Enter username"))
        self.username_entry.bind("<FocusOut>", lambda event: self.add_placeholder(self.username_entry, "Enter username"))
        tk.Button(self.master, text="Create User", command=self.create_user).pack(pady=5)

        tk.Label(self.master, text="Add Event").pack(pady=5)
    
        self.event_username_entry = tk.Entry(self.master, width=30)
        self.event_username_entry.pack(pady=5)
        self.event_username_entry.insert(0, "Enter username")
        self.event_username_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(self.event_username_entry, "Enter username"))
        self.event_username_entry.bind("<FocusOut>", lambda event: self.add_placeholder(self.event_username_entry, "Enter username"))
        
        self.event_day_entry = tk.Entry(self.master, width=30)
        self.event_day_entry.pack(pady=5)
        self.event_day_entry.insert(0, "Enter day (e.g., Monday)")
        self.event_day_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(self.event_day_entry, "Enter day (e.g., Monday)"))
        self.event_day_entry.bind("<FocusOut>", lambda event: self.add_placeholder(self.event_day_entry, "Enter day (e.g., Monday)"))
        
        self.event_entry = tk.Entry(self.master, width=30)
        self.event_entry.pack(pady=5)
        self.event_entry.insert(0, "Enter event details")
        self.event_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(self.event_entry, "Enter event details"))
        self.event_entry.bind("<FocusOut>", lambda event: self.add_placeholder(self.event_entry, "Enter event details"))
        
        tk.Button(self.master, text="Add Event", command=self.add_event).pack(pady=5)

        tk.Label(self.master, text="List Events").pack(pady=5)
        self.list_username_entry = tk.Entry(self.master, width=30)
        self.list_username_entry.pack(pady=5)
        self.list_username_entry.insert(0, "Enter username")
        self.list_username_entry.bind("<FocusIn>", lambda event: self.clear_placeholder(self.list_username_entry, "Enter username"))
        self.list_username_entry.bind("<FocusOut>", lambda event: self.add_placeholder(self.list_username_entry, "Enter username"))
        tk.Button(self.master, text="List Events", command=self.list_events).pack(pady=5)

        tk.Label(self.master, text="Output").pack(pady=5)
        self.output_text = tk.Text(self.master, height=10, width=50)
        self.output_text.pack(pady=5)

    def send_command(self, command):
        try:
            self.client_socket.sendall(command.encode("utf-8"))
            response = self.client_socket.recv(1024).decode("utf-8")
            return response
        except Exception as e:
            return f"Error: {e}"

    def create_user(self):
        username = self.username_entry.get()
        if not username:
            messagebox.showwarning("Input Error", "Please enter a username")
            return

        response = self.send_command(f"create_user {username}")
        messagebox.showinfo("Server Response", response)

    def add_event(self):
        username = self.event_username_entry.get()
        day = self.event_day_entry.get()
        event = self.event_entry.get()

        if not username or not day or not event:
            messagebox.showwarning("Input Error", "Please enter username, day, and event")
            return

        response = self.send_command(f"add_event {username} {day} {event}")
        messagebox.showinfo("Server Response", response)

    def list_events(self):
        username = self.list_username_entry.get()
        if not username:
            messagebox.showwarning("Input Error", "Please enter a username")
            return

        response = self.send_command(f"list_events {username}")
        
        self.output_text.delete("1.0", tk.END)
        
        self.output_text.insert(tk.END, response + "\n")

    def clear_placeholder(self, entry, placeholder):
        if entry.get() == placeholder:
            entry.delete(0, tk.END)
            entry.config(fg="black")
    
    def add_placeholder(self, entry, placeholder):
        if not entry.get():
            entry.insert(0, placeholder)
            entry.config(fg="grey")

    def close_connection(self):
        try:
            self.client_socket.close()
        except Exception as e:
            print(f"Error closing socket: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = CalendarClientGUI(root)
    root.protocol("WM_DELETE_WINDOW", lambda: (app.close_connection(), root.destroy()))
    root.mainloop()
