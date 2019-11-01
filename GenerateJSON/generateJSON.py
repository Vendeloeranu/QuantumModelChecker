import json
import sys
import subprocess

PRISM = "PRISM"
JANI = "JANI"
json_file = "json.json"

model_file_tmp = "model_tmp.tmp"
property_file_tmp = "property_tmp.tmp"


def main():
	model = sys.argv[1]
	prop = sys.argv[2]
	model_type = sys.argv[3]

	write2file(model_file_tmp, (model))
	write2file(property_file_tmp, (prop))

	cmd = "java -jar epmc-qmc.jar check --model-input-files "+model_file_tmp +" --property-input-files "+property_file_tmp+" --model-input-type "
	if model_type == PRISM:
		cmd += "prism-qmc"
	elif model_type == JANI:
		cmd += "jani-qmc"
	s = subprocess.getstatusoutput(cmd)

	result = {}
	result["status_code"] = s[0]
	if (s[0] == 0):
		parse_result = parseOut(s[1])

		result["output"] = parse_result[0]
		result["result"] = parse_result[1]
	else:
		result["output"] = s[1]
		result["result"] = ""

	print(result["output"])
	print(result["result"])

	json.dump(result,open(json_file,"w"))

def parseOut(out):
	pos = out.find("=====================================================");
	return [out[0:pos], out[pos:]]
def write2file(fileName, content):
	file = open(fileName,"w")
	file.write(str(content))

if __name__ == '__main__':
	main()
