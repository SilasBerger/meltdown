use std::cmp::min;
use std::io::{Read, Write};
use std::time::Duration;
use serialport;
use serialport::{COMPort, SerialPort};
use serde::Deserialize;

fn main() {
    let input_port_result = serialport::new("COM3", 9600).open_native();
    if input_port_result.is_err() {
        eprint!("Error opening input COM port: {:?}", input_port_result.err());
        return;
    }

    let output_port_result = serialport::new("COM5", 9600).open_native();
    if output_port_result.is_err() {
        eprint!("Error opening output COM port: {:?}", output_port_result.err());
        return;
    }

    let mut bridge = ArduinoBridge::new(input_port_result.unwrap(), output_port_result.unwrap());
    bridge.open();
}

struct ArduinoBridge {
    input_port: COMPort,
    output_port: COMPort,
}

impl ArduinoBridge {

    fn new(input_port: COMPort, output_port: COMPort) -> Self {
        ArduinoBridge {
            input_port,
            output_port,
        }
    }

    fn open(&mut self) {
        self.input_port.write_data_terminal_ready(true).unwrap();
        self.input_port.set_timeout(Duration::from_secs(5)).unwrap();
        let mut data_buffer: Vec<u8> = vec![];
        loop {
            let bytes_to_read = self.input_port.bytes_to_read().unwrap();
            if bytes_to_read > 0 {
                let mut read_buffer: Vec<u8> = vec![0; min(usize::MAX as u64, bytes_to_read as u64) as usize];
                self.input_port.read_exact(&mut read_buffer).unwrap();

                for data_byte in read_buffer {
                    match data_byte {
                        2 => data_buffer = vec![],
                        4 => {
                            self.handle_device_input(data_buffer);
                            data_buffer = vec![];
                        },
                        character => data_buffer.push(character)
                    }
                }
            }
        }
    }

    fn handle_device_input(&mut self, data_buffer: Vec<u8>) {
        let device_input = self.parse_input(data_buffer);
        println!("{:?}", device_input);
        match device_input {
            DeviceInput::Buttons { index } => {
                self.output_port.write_all(&vec![index as u8]).unwrap();
                self.output_port.flush().unwrap();
            },
            _ => ()
        }
    }

    fn parse_input(&self, bytes: Vec<u8>) -> DeviceInput {
        let string_message = String::from_utf8(bytes).unwrap();
        serde_json::from_str(&string_message).unwrap()
    }
}



fn scan_devices() {
    println!("{:?}", serialport::available_ports());
}

#[derive(Debug, Deserialize)]
#[serde(tag = "device")]
enum DeviceInput {
    #[serde(rename = "keypad")]
    Keypad {
        value: String,
    },
    #[serde(rename = "pots")]
    Potentiometers {
        index: i32,
        value: i32,
    },
    #[serde(rename = "buttons")]
    Buttons {
        index: i32,
    }
}
