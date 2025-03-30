pub extern crate tracks_rs;

use log::{error, info, LevelFilter};
use std::panic::PanicHookInfo;
use android_logger::Config;
use std::backtrace::Backtrace;

#[ctor::ctor]
fn main() {
    android_logger::init_once(Config::default().with_max_level(LevelFilter::Trace));

    std::panic::set_hook(panic_hook(true, true));
}

/// Returns a panic handler, optionally with backtrace and spantrace capture.
pub fn panic_hook(
    backtrace: bool,
    spantrace: bool,
) -> Box<dyn Fn(&PanicHookInfo) + Send + Sync + 'static> {
    // Mostly taken from https://doc.rust-lang.org/src/std/panicking.rs.html
    Box::new(move |info| {
        let location = info.location().unwrap();
        let msg = match info.payload().downcast_ref::<&'static str>() {
            Some(s) => *s,
            None => match info.payload().downcast_ref::<String>() {
                Some(s) => &s[..],
                None => "Box<dyn Any>",
            },
        };

        info!(target: "panic", "panicked at '{}', {}", msg, location);
        if backtrace {
            error!(target: "panic", "{:?}", Backtrace::force_capture());
        }
        if spantrace {
            // error!(target: "panic", "{:?}", SpanTrace::capture());
        }
    })
}
