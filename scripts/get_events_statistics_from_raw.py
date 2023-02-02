import os
import numpy as np
import cv2
from matplotlib import pyplot as plt

from absl import app
from absl import flags

from metavision_core.event_io.raw_reader import RawReader
from metavision_core.event_io.py_reader import EventDatReader

FLAGS = flags.FLAGS
flags.DEFINE_string("raw_file", None, "The raw file with the events.")
flags.DEFINE_integer("seconds", None, "Duration [s]")
flags.DEFINE_integer("ms", None, "Duration [ms]")
flags.DEFINE_integer("us", None, "Duration [us]")

# Required flag.
flags.mark_flag_as_required("raw_file")
flags.mark_flag_as_required("seconds")
flags.mark_flag_as_required("ms")
flags.mark_flag_as_required("us")


def viz_events(events, height, width):
    img = np.full((height, width, 3), 128, dtype=np.uint8)
    img[events['y'], events['x']] = 255 * events['p'][:, None]
    return img


def main(argv):
    del argv  # Unused.

    record_raw = RawReader(FLAGS.raw_file)
    height, width = record_raw.get_size()
    seconds = FLAGS.seconds + FLAGS.ms * 1E-3 + FLAGS.us * 1E-6
    print("height: " + str(height) + ", width: " + str(width))
    print(record_raw)

    pos_events_per_pixel = np.zeros((height, width))
    neg_events_per_pixel = np.zeros((height, width))
    total_events_per_pixel = np.zeros((height, width))
    print("events_per_pixel.shape: " + str(total_events_per_pixel.shape))

    while not record_raw.is_done() and record_raw.current_event_index() < 1E7:
        # load the next 50 ms worth of events
        events = record_raw.load_delta_t(50000)
        im = viz_events(events, height, width)

        for event in events:
            total_events_per_pixel[event['y'], event['x']] += 1

            if event['p'] > 0:
                pos_events_per_pixel[event['y'], event['x']] += 1
            else:
                neg_events_per_pixel[event['y'], event['x']] += 1


    total_events_per_pixel = cv2.flip(total_events_per_pixel, 0)
    total_events_per_pixel = cv2.flip(total_events_per_pixel, 1)

    pos_events_per_pixel = cv2.flip(pos_events_per_pixel, 0)
    pos_events_per_pixel = cv2.flip(pos_events_per_pixel, 1)

    neg_events_per_pixel = cv2.flip(neg_events_per_pixel, 0)
    neg_events_per_pixel = cv2.flip(neg_events_per_pixel, 1)

    cv2.imwrite("events_per_pixel_from_raw.png", total_events_per_pixel)
    print("events_per_pixel.shape: " + str(total_events_per_pixel.shape))
    fs = cv2.FileStorage("events_per_pixel_from_raw.json", cv2.FILE_STORAGE_WRITE)
    fs.write("total_events_per_pixel", total_events_per_pixel)
    fs.write("pos_events_per_pixel", pos_events_per_pixel)
    fs.write("neg_events_per_pixel", neg_events_per_pixel)

    total_events_per_pixel_per_second = cv2.multiply(total_events_per_pixel, 1 / seconds)
    pos_events_per_pixel_per_second = cv2.multiply(pos_events_per_pixel, 1 / seconds)
    neg_events_per_pixel_per_second = cv2.multiply(neg_events_per_pixel, 1 / seconds)
    cv2.imwrite("events_per_pixel_per_second_from_raw.png", total_events_per_pixel_per_second)
    print("events_per_pixel_per_second.shape: " + str(total_events_per_pixel_per_second.shape))
    fs.write("total_events_per_pixel_per_second", total_events_per_pixel_per_second)
    fs.write("pos_events_per_pixel_per_second", pos_events_per_pixel_per_second)
    fs.write("neg_events_per_pixel_per_second", neg_events_per_pixel_per_second)
    fs.release()


if __name__ == '__main__':
    app.run(main)
