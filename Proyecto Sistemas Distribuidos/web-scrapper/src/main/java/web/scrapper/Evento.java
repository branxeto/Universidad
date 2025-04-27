package web.scrapper;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.databind.JsonNode;

@JsonIgnoreProperties({"alerts"})
public class Evento {
    private long endTimeMillis;
    private long startTimeMillis;
    private String startTime;
    private String endTime;
    private JsonNode jams;
    //private JsonNode alerts;

    public Evento() {}

    // <--- GETTERS --->
    public long getEndTimeMillis() {
        return this.endTimeMillis;
    }
    public long getStartTimeMillis() {
        return this.startTimeMillis;
    }
    public String getStartTime() {
        return this.startTime;
    }
    public String getEndTime() {
        return this.endTime;
    }
    public JsonNode getJams() {
        return this.jams;
    }
    //public JsonNode getAlerts() {
    //    return this.alerts;
    //}


    // <--- SETTERS --->
    public void setEndTimeMillis(long endTimeMillis) {
        this.endTimeMillis = endTimeMillis;
    }
    public void setStartTimeMillis(long startTimeMillis) {
        this.startTimeMillis = startTimeMillis;
    }
    public void setStartTime(String startTime) {
        this.startTime = startTime;
    }
    public void setEndTime(String endTime) {
        this.endTime = endTime;
    }
    public void setJams(JsonNode jams) {
        this.jams = jams;
    } 
    //public void setAlerts(JsonNode alerts) {
    //    this.alerts = alerts;
    //}    
}
