package web.scrapper;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.bson.Document;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.mongodb.client.MongoClient;
import com.mongodb.client.MongoClients;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;
import com.mongodb.client.result.InsertOneResult;

public class WebScrapper {
    public static void main(String[] args) throws Exception {
        
        ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
        scheduler.scheduleAtFixedRate(() -> {
            try {
                ejecutarTarea();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }, 0, 1, TimeUnit.MINUTES);

    }

    public static void ejecutarTarea() throws Exception {
        String json = ""; //Aqui se va a guardar el JSON crudo
        ObjectMapper objectMapper = new ObjectMapper();

        // <--- CONSULTA HTTP --->
        try {
            //Construir Request
            HttpRequest request = HttpRequest.newBuilder()
                .uri(new URI("https://www.waze.com/live-map/api/georss?top=-32.9167&bottom=-34.3167&left=-71.7167&right=-69.7833&env=row&types=alerts,traffic"))
                .GET()
                .header("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/132.0.0.0 Safari/537.36 OPR/117.0.0.0")
                .build();

            // Enviar y recibir respuesta que el cliente mando (Request)
            HttpResponse<String> response = HttpClient.newHttpClient()
                .send(request, HttpResponse.BodyHandlers.ofString());
            json = response.body(); 
            //System.out.println(json);
        } catch (IOException e) {
            throw new RuntimeException(e);
        } catch (URISyntaxException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        // <--- MANEJAR FORMATO JSON --->
        Evento evento = objectMapper.readValue(json, Evento.class);
        String eventoString = objectMapper.writeValueAsString(evento);
        System.out.println("Evento: " + eventoString);

        // <--- Base de Datos --->
        String uri = "mongodb+srv://Branco:Branco1323@universidad.wavnkf6.mongodb.net/?retryWrites=true&w=majority&appName=Universidad";
        try (MongoClient mongoClient = MongoClients.create(uri)) {
            MongoDatabase database = mongoClient.getDatabase("Sistemas_Distribuidos");
            MongoCollection<Document> collection = database.getCollection("Datos");
            Document doc = Document.parse(eventoString); 
            if (doc != null) {
                InsertOneResult result = collection.insertOne(doc);
                System.out.println("Documento insertado con ID: " + result.getInsertedId());
            } else {
                System.out.println("Hubo un error en la obtención del JSON.");
            }
        }
    }
}
