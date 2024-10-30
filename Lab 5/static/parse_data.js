if (!!window.EventSource) {
    var source = new EventSource('/');
    source.onmessage = function(e) {
      var bumper = e.data[1]
      var cliff = e.data[3];
      var drop = e.data[5];


      // finish the code to handle the bumper status
        if (bumper=="0")
          {
            document.getElementById("bumpertoggle").value = "OFF";
          }
        if (bumper=="1")
        {
          document.getElementById("bumpertoggle").value = "Right";
        }
        if (bumper == "2")
          {
            document.getElementById("bumpertoggle").value = "Center";
          }
        if (bumper == "4")
        {
          document.getElementById("bumpertoggle").value = "Left";
        }
  
        
         
        
        // finish the code to handle the wheel drop status 
      if (drop=="0")
        {
          document.getElementById("wheeldroptoggle").value = "OFF";
        }
      if (drop=="1")
        {
          document.getElementById("wheeldroptoggle").value = "Right";
        }
      if (drop=="2")
        {
          document.getElementById("wheeldroptoggle").value = "Left";
        }
      

      // finish the code to handle cliff status 
      if (cliff=="0")
        {
          document.getElementById("clifftoggle").value = "OFF";
        }
      else {
          document.getElementById("clifftoggle").value = "ON";
      }
      


    }
  }