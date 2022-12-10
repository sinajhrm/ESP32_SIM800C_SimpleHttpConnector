class SMS {
	constructor(pduString) {
		this.decodedArray = pduDecoder(pduString)
		if (this.decodedArray == 'Invalid PDU String!') {
			this.decodedArray = false;
		}
		else {
			this.SMS_Text = this.findProp("User Data");
			this.sender = this.findProp("(hideable)Number");
			this.timeStamp = new Date(this.findProp("(hideable)Service Centre Time Stamp").replace( " +3.5","+03:30"));
		}
	}

	findProp(targetProp) {
		for (let i = 0; i < this.decodedArray.length; i++)
			if (this.decodedArray[i].includes(targetProp + "\t"))
				return this.decodedArray[i].replace(targetProp + "\t","")
		return false;
	}
}

function RequestSMSsListAndUpdateUI() {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function () {
		if (this.readyState == 4 && this.status == 200) {
			SMSsList=[]
			// <p id="RawCMGLText">
			// <p id="ExtractedPDUs">
			// <p id="DecodedSMSs">
			document.getElementById("RawCMGLText").innerHTML = this.responseText;

			CMGLResponseSplits = this.responseText.replace("AT+CMGL=4,1\r\r\n", "").replace("\r\nOK", "").split("+CMGL: ");

			CMGLResponseSplits.splice(0, 1)

			document.getElementById("ExtractedPDUs").innerHTML = "";
			document.getElementById("DecodedSMSs").innerHTML = "";

			CMGLResponseSplits.forEach(pduSMS => {
				pduHexString = pduSMS.split("\n").at(-3).trim();
				SMSsList.push(new SMS(pduHexString));
				decodedPdu = pduDecoder(pduHexString);
				document.getElementById("ExtractedPDUs").innerHTML += pduSMS.trim() + "<br>********************<br>";
				document.getElementById("DecodedSMSs").innerHTML += decodedPdu.at(-1).replace("User Data\t", "") + "<br>********************<br>";
			})
		}
	};
	xhttp.open("GET", "/get_pdu_smss", true);
	xhttp.send();
}
let SMSsList = []

RequestSMSsListAndUpdateUI()

setInterval(RequestSMSsListAndUpdateUI, 5 * 1000);