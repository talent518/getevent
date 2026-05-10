#!/usr/bin/env python
#
# Copyright (C) 2015 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the 'License');
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an 'AS IS' BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# pylint: disable=bad-indentation,bad-continuation

import os
import re
import sys

input_prop_list = []
ev_list = []
syn_list = []
key_list = []
rel_list = []
abs_list = []
sw_list = []
msc_list = []
led_list = []
rep_list = []
snd_list = []
mt_tool_list = []
ff_status_list = []
ff_list = []

r = re.compile(r'#define\s+(\S+)\s+((?:0x)?\d+)')

for file in ['/usr/include/linux/input-event-codes.h', '/usr/include/linux/input.h']:
  with open(file, 'r') as f:
    for line in f:
      m = r.match(line)
      if m:
        name = m.group(1)
        if name.startswith("INPUT_PROP_"):
          input_prop_list.append(name)
        elif name.startswith("EV_"):
          ev_list.append(name)
        elif name.startswith("SYN_"):
          syn_list.append(name)
        elif name.startswith("KEY_") or name.startswith("BTN_"):
          key_list.append(name)
        elif name.startswith("REL_"):
          rel_list.append(name)
        elif name.startswith("ABS_"):
          abs_list.append(name)
        elif name.startswith("SW_"):
          sw_list.append(name)
        elif name.startswith("MSC_"):
          msc_list.append(name)
        elif name.startswith("LED_"):
          led_list.append(name)
        elif name.startswith("REP_"):
          rep_list.append(name)
        elif name.startswith("SND_"):
          snd_list.append(name)
        elif name.startswith("MT_TOOL_"):
          mt_tool_list.append(name)
        elif name.startswith("FF_STATUS_"):
          ff_status_list.append(name)
        elif name.startswith("FF_"):
          ff_list.append(name)

def Dump(f, struct_name, values):
  f.write('static struct label %s[] = {\n' % (struct_name))
  for value in values:
    f.write('    LABEL(%s),\n' % (value))
  f.write('    LABEL_END,\n')
  f.write('};\n')

with open(sys.argv[1], 'w') as f:
  Dump(f, "input_prop_labels", input_prop_list)
  Dump(f, "ev_labels", ev_list)
  Dump(f, "syn_labels", syn_list)
  Dump(f, "key_labels", key_list)
  Dump(f, "rel_labels", rel_list)
  Dump(f, "abs_labels", abs_list)
  Dump(f, "sw_labels", sw_list)
  Dump(f, "msc_labels", msc_list)
  Dump(f, "led_labels", led_list)
  Dump(f, "rep_labels", rep_list)
  Dump(f, "snd_labels", snd_list)
  Dump(f, "mt_tool_labels", mt_tool_list)
  Dump(f, "ff_status_labels", ff_status_list)
  Dump(f, "ff_labels", ff_list)

