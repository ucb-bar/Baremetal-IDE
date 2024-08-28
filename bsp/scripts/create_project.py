import os
import shutil
import re
import xml
import json

import jinja2
# import xmljson
import sys


class ProjectBuilder:
    IDE_BUILDER_VERSION = "0.0.1"

    TEMPLATE_PATH = os.getcwd()+"/bsp/scripts/templates"

    PROJECT_ROOT = os.getcwd()+"/bsp/"
    JSON_CONFIG_FOLDER = os.path.dirname(__file__) + "/json_description"

    def __init__(self, chipname):
        self.chipname = chipname
        
        self.jinja_template_loader = jinja2.FileSystemLoader(searchpath=self.TEMPLATE_PATH)
        self.jinja_template_env = jinja2.Environment(loader=self.jinja_template_loader)
        # self.chip_config = json.load(open(self.JSON_CONFIG_FOLDER+"/"+CHIP_NAME+"_description.json"))
        # assert " " not in self.chip_config.get("name")
        # assert "-" not in self.chip_config.get("name")

        # self.chipname_capitalized = self.chip_config.get("name")
        # assert self.chipname_capitalized.lower() == self.chipname
    
    @property
    def version(self):
        return self.IDE_BUILDER_VERSION
        
    def clean(self):
        try:
            shutil.rmtree(self.PROJECT_ROOT)
        except FileNotFoundError:
            pass

    def buildBSPDirectories(self):
        bsp_folder_path = os.path.join(self.PROJECT_ROOT, self.chipname)
        os.makedirs(os.path.join(bsp_folder_path, "inc"), exist_ok=True)
        os.makedirs(os.path.join(bsp_folder_path, "src"), exist_ok=True)
        os.makedirs(os.path.join(bsp_folder_path, "boot"), exist_ok=True)

    def generateFromTemplate(self, generated_path, template_path, **kwargs):
        template = self.jinja_template_env.get_template(template_path)
        content = template.render(**kwargs)

        with open(generated_path, "w") as f:
            f.write(content)

    def replace(self, generated_path, marker, replace):
        with open(generated_path, 'r') as f:
            content = f.read()
        content = content.replace(marker, replace)
        with open(generated_path, 'w') as f:
            f.write(content)
        f.close()

    def generateBspFiles(self):
        # startup
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "boot", "bootrom.S"), 
            "bootrom.S"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "boot", "startup.S"), 
            "startup.S"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "Makefile"), 
            "Makefile"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, self.chipname + ".h"), 
            "map.h"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "inc/hal_rcc.h"), 
            "hal_rcc.h"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "src/hal_rcc.c"), 
            "hal_rcc.c"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, "inc/ll_rcc.h"), 
            "rcc.h"
            )
        self.generateFromTemplate(
            os.path.join(self.PROJECT_ROOT, self.chipname, self.chipname+".ld"), 
            "temp.ld"
            )
        
        makefile_path = os.path.join(self.PROJECT_ROOT, self.chipname, "Makefile")
        self.replace(makefile_path, '[chip]', self.chipname)

        header_path = os.path.join(self.PROJECT_ROOT, self.chipname, self.chipname + ".h")
        self.replace(header_path, '[chip]', self.chipname.upper())

        with open(os.path.join(self.PROJECT_ROOT, self.chipname, self.chipname + ".c"), "x") as f:
            f.write(f"#include <{self.chipname}.h>")
        f.close()

    def generateDebug(self):
        debug_folder_path = os.path.join(self.PROJECT_ROOT, self.chipname)

        jtag_cfg_path = os.path.join(debug_folder_path, self.chipname+".cfg")
        jtag_svd_path = os.path.join(debug_folder_path, self.chipname+".svd")

        self.generateFromTemplate(
            jtag_cfg_path,
            "chipname.cfg"
            )
        
        # json_data = {
        #     "name": {"$t": self.chipname_capitalized},
        #     "version": {"$t": self.IDE_BUILDER_VERSION},
        #     "description": {"$t": "description_text"},
        #     "cpu": {
        #         "name": {"$t": self.chip_config.get("harts")[0].get("name")},
        #         "revision": {"$t": self.chip_config.get("harts")[0].get("arch")},
        #         "endian": {"$t": "little"},
        #         "mpuPresent": {"$t": "false"},
        #         "fpuPresent": {"$t": self.chip_config.get("harts")[0].get("arch").find("f") != -1},
        #         "nvicPrioBits": {"$t": 1},
        #         "vendorSystickConfig": {"$t": True},
        #     },
        #     # Define the number of data bits uniquely selected by each address. 
        #     "addressUnitBits": {"$t": 8},
        #     # Define the number of data bit-width  of the maximum single data transfer supported by the bus infrastructure.
        #     "width": {"$t": re.findall("\d+", self.chip_config.get("harts")[0].get("arch"))[0]},
        #     # Default bit-width of any register contained in the device.
        #     "size": {"$t": 32},
        #     # Default access rights for all registers.
        #     "access": {"$t": "read-write"},
        #     # Default access protection for all registers.
        #     #"protection": {"$t": "read-write"},
        #     # Default value for all registers at RESET.
        #     "resetValue": {"$t": "0x00000000"},
        #     # Define which register bits have a defined reset value.
        #     "resetMask": {"$t": "0xFFFFFFFF"},

        #     "peripherals": {
        #         "peripheral": [{
        #             "name": {"$t": periph.get("name")},
        #             "description": {"$t": periph.get("description")},
        #             "groupName": {"$t": periph.get("group")},
        #             "baseAddress": {"$t": periph.get("baseAddress")},
        #             "addressBlock": {
        #                 "offset": {"$t": periph.get("addressBlock").get("offset")},
        #                 "size": {"$t": periph.get("addressBlock").get("size")},
        #                 "usage": {"$t": periph.get("addressBlock").get("usage")},
        #             },
        #             "registers": {}
        #         } for periph in self.chip_config.get("peripherals")]
        #     },
        # }


        
        # xml_data = xmljson.gdata.etree(json_data, root=xml.etree.ElementTree.Element("device", attrib={
        #     "xmlns:xs": "http://www.w3.org/2001/XMLSchema-instance",
        #     "schemaVersion": "1.1",
        #     "xs:noNamespaceSchemaLocation": "CMSIS-SVD_Schema_1_1.xsd"
        # }))

        # xml_data = xml.etree.ElementTree.tostring(xml_data, encoding="unicode")
        # # print(xml_data)

        # with open(jtag_svd_path, "w") as f:
        #     f.write(xml_data)
        

if __name__ == "__main__":
    # Accept arguments from command line 
    if len(sys.argv) != 2:
        print("Missing chip's name argument")
        sys.exit(1)

    CHIP_NAME = sys.argv[1]
    
    if CHIP_NAME in os.listdir(os.getcwd()+"/bsp/"):
        print("Chip project already exist.")
        sys.exit(1)

    builder = ProjectBuilder(CHIP_NAME)
    builder.buildBSPDirectories()
    builder.generateBspFiles()
    builder.generateDebug()
