use std::cmp::min;
use std::io::Read;
use std::time::Duration;
use serialport;
use serialport::{COMPort, SerialPort};
use serde::Deserialize;

fn main() {
    let result = serialport::new("COM3", 9600).open_native();
    match result {
        Ok(serial_port) => read_data(serial_port),
        Err(error) => println!("Error opening COM port: {}", error)
    }
}

fn read_data(mut serial_port: COMPort) {
    serial_port.write_data_terminal_ready(true).unwrap();
    serial_port.set_timeout(Duration::from_secs(5)).unwrap();
    let mut data_buffer: Vec<u8> = vec![];
    loop {
        let bytes_to_read = serial_port.bytes_to_read().unwrap();
        if bytes_to_read > 0 {
            let mut read_buffer: Vec<u8> = vec![0; min(usize::MAX as u64, bytes_to_read as u64) as usize];
            serial_port.read_exact(&mut read_buffer).unwrap();

            for data_byte in read_buffer {
                match data_byte {
                    2 => data_buffer = vec![],
                    4 => {
                        let message = parse_message(data_buffer);
                        println!("{:?}", message);
                        data_buffer = vec![];
                    },
                    character => data_buffer.push(character)
                }
            }
        }
    }
}

fn parse_message(bytes: Vec<u8>) -> InputMessage {
    let string_message = String::from_utf8(bytes).unwrap();
    serde_json::from_str(&string_message).unwrap()
}

fn scan_devices() {
    println!("{:?}", serialport::available_ports());
}

#[derive(Debug, Deserialize)]
struct InputMessage {
    device: String,
    #[serde(rename = "inputType")]
    input_type: String,
    value: String,
}